require 'rake/clean'
require "#{ENV['CORTO_BUILD']}/version"
require "#{ENV['CORTO_BUILD']}/libmapping"

CHDIR_SET ||= false

if !CHDIR_SET then
    Dir.chdir(File.dirname(Rake.application.rakefile))
end

if not ENV['CORTO_BUILD'] then
    raise "CORTO_BUILD not defined (did you forget to 'source configure'?)"
end
if not defined? ARTEFACT then
    raise "artefact: ARTEFACT not specified\n"
end
if not ENV['target'] then
    ENV['target'] = "debug"
end

# --- DATA PROCESSING

# Utility that replaces buildsystem tokens with actual values
def corto_replace(str)
    str = str.gsub("$(CORTO_OS)", `uname -s`[0...-1])
    str = str.gsub("$(CORTO_PLATFORM)", `uname -p`[0...-1])
    str = str.gsub("$(CORTO_TARGET)", TARGETDIR)
    etcPath = ENV['CORTO_TARGET'] + "/corto/#{VERSION}"
    etcPath = TARGETDIR[etcPath.length..-1]
    str = str.gsub("$(CORTO_ETC)", ENV['CORTO_TARGET'] + "/etc/corto" + etcPath)
end

# Public variables
INCLUDE ||= ["include"]
LIB ||= []
LIBPATH ||= []
LINK ||= []
CFLAGS ||= []
CXXFLAGS ||= []
LFLAGS ||= []
USE_PACKAGE ||= []
USE_COMPONENT ||= []
USE_LIBRARY ||= []
DEFINE ||= []
CC ||= if ENV['CC'].nil? or ENV['CC'].empty?
  "cc"
else
  ENV['CC']
end
CXX ||= if ENV['CXX'].nil? or ENV['CXX'].empty?
  "g++"
else
  ENV['CXX']
end

# Private variables
TARGETDIR ||= ENV['CORTO_TARGET'] + "/lib"
GENERATED_SOURCES ||= []
GENERATED_HEADERS ||= []
USE_PACKAGE_LOADED ||=[]
USE_COMPONENT_LOADED ||=[]

# Add default include paths
INCLUDE <<
    "src" <<
    "#{ENV['CORTO_TARGET']}/include/corto/#{VERSION}/packages" <<
    "/usr/local/include/corto/#{VERSION}/packages"

# Default CFLAGS
CFLAGS << "-std=c99" << "-Wstrict-prototypes" << "-pedantic" << "-fPIC" << "-D_XOPEN_SOURCE=600"
CFLAGS.unshift("-Wall")

# Default CXXFLAGS
CXXFLAGS << "-Wall" << "-std=c++11"

# Set NDEBUG macro in release builds to disable tracing & checking
# Also enable optimizations
if ENV['target'] == "release" then
  CFLAGS << "-DNDEBUG" << "-O3"
  CXXFLAGS << "-DNDEBUG" << "-O3"
end

# Enable debug information in debug builds & disable optimizations
if ENV['target'] == "debug" then
  CFLAGS << "-g" << "-O0"
  CXXFLAGS << "-g" << "-O0"
end

# Crawl src directory to get list of source files
SOURCES = Rake::FileList["src/**/*.{c,cc,cpp,cxx}"]
OBJECTS = SOURCES.ext(".o").pathmap(".corto/%{^src/,obj/}p") +
          Rake::FileList[GENERATED_SOURCES].ext(".o").pathmap(".corto/obj/%f")

# Load components from file
if File.exists? ".corto/components.txt" then
    f = File.open(".corto/components.txt")
    f.each_line {|line|
        USE_COMPONENT_LOADED.push line[0...-1]
        USE_COMPONENT.push line[0...-1]
    }
end

# Load packages from file
if File.exists? ".corto/packages.txt" then
    f = File.open(".corto/packages.txt")
    f.each_line {|line|
        USE_PACKAGE_LOADED.push line[0...-1]
        USE_PACKAGE.push line[0...-1]
    }
end

# Setup default clean & clobber rules
CLEAN.include(".corto/obj")
CLEAN.include("doc")
CLOBBER.include(TARGETDIR + "/" + ARTEFACT)
CLOBBER.include(GENERATED_SOURCES)
CLOBBER.include(GENERATED_HEADERS)

# If components.txt is empty, clobber it
if USE_COMPONENT_LOADED.length == 0 then
    CLOBBER.include(".corto/components.txt")
end

# If packages.txt is empty, clobber it
if USE_PACKAGE_LOADED.length == 0 then
    CLOBBER.include(".corto/packages.txt")
end


# --- BUILD RULES

# Task that collects all artefacts from the build and store them in a 'pack'
# folder, where they can be tarred for redistribution.
task :collect do
    verbose(false)
    artefact = "#{TARGETDIR}/#{ARTEFACT}"
    target = ENV['HOME'] + "/.corto/pack" + artefact["#{ENV['CORTO_TARGET']}".length..artefact.length]
    sh "mkdir -p " + target.split("/")[0...-1].join("/")

    if `uname` == "Darwin\n" then
        sh "cp -r #{artefact} #{target}"
    else
        sh "cp -rL #{artefact} #{target}"
    end
end

# Rule to automatically create components.txt
file ".corto/components.txt" do
    verbose(false)
    sh "mkdir -p .corto"
    sh "touch .corto/components.txt"
end

# Rule to automatically create packages.txt
file ".corto/packages.txt" do
    verbose(false)
    sh "mkdir -p .corto"
    sh "touch .corto/packages.txt"
end

task :binary => "#{TARGETDIR}/#{ARTEFACT}"

# Build artefact
file "#{TARGETDIR}/#{ARTEFACT}" => OBJECTS do
    verbose(false)
    sh "mkdir -p #{TARGETDIR}"

    # Create list of files that are going to be linked with. Abstract away from the
    # difference between dll, so and dylib. When a dylib is encountered, perform  a
    # bit of magic to ensure that the executable can find the shared object.
    LINKED = LINK.map do |l|
        l = corto_replace(l)
        lib = File.dirname(l) + "/lib" + File.basename(l) + ".so"
        if (not File.exists? lib) and (`uname` == "Darwin\n") then
            lib = File.dirname(l) + "/lib" + File.basename(l) + ".dylib"
            if (not File.exists? lib) then
                abort "\033[1;31m[ #{l} not found ]\033[0;49m"
            end
        end
        lib
    end

    OBJECTS.concat(LINKED)
    objects  = "#{OBJECTS.to_a.uniq.join(' ')}"
    cflags = "#{CFLAGS.join(" ")}"
    libpath = "#{LIBPATH.map {|i| "-L" + corto_replace(i)}.join(" ")} "
    libmapping = "#{(LibMapping.mapLibs(LIB)).map {|i| "-l" + i}.join(" ")}"
    lflags = "#{LFLAGS.join(" ")} -o #{TARGETDIR}/#{ARTEFACT}"
    use_link =
        USE_PACKAGE.map do |i|
            if i == "corto" then
                "#{ENV['CORTO_HOME']}/lib/corto/#{VERSION}/packages/corto/libcorto.so"
            else
                result = `corto locate #{i} --lib`[0...-1]
                if $?.exitstatus != 0 then
                    p result
                    abort "\033[1;31m[ build failed ]\033[0;49m"
                end
                result
            end
        end.join(" ") + " " +
        USE_COMPONENT.map {|i| "#{ENV['CORTO_HOME']}/lib/corto/#{VERSION}/components/lib" + i + ".so"}.join(" ") + " " +
        USE_LIBRARY.map {|i| "#{ENV['CORTO_HOME']}/lib/corto/#{VERSION}/libraries/lib" + i + ".so"}.join(" ")

    # Check if there were any new files created during code generation
    Rake::FileList["src/*.c"].each do |file|
        obj = file.ext(".o").pathmap(".corto/obj/%f")
        if not OBJECTS.include? obj
            build_source(file, obj, true)
            objects += " " + obj
        end
    end

    cc_command = "#{CC} #{objects} #{cflags} #{libpath} #{libmapping} #{use_link} #{lflags}"
    begin
      sh cc_command
    rescue
      puts "\033[1;31mcommand failed: #{cc_command}\033[0;49m"
      abort()
    end

    # If required, alter paths to dylib files
    LINKED.each do |lib|
        if File.extname(lib) == ".dylib" then
            # Obtain name of library, check if its different from target path. If
            # names don't match, setup a task to alter the name in the executable
            # path.
            libname = `otool -L #{lib}`.split("\n")[1].split[0]
            if libname != lib then
                sh "install_name_tool -change #{libname} #{lib} #{TARGETDIR}/#{ARTEFACT}"
            end
        end
    end

    # If target is release, strip binary
    if ENV['target'] == "release" then
      sh "strip -Sx #{TARGETDIR}/#{ARTEFACT}"
    end

    if ENV['target'] == "release" then
      c_bold = "\033[1;36m"
      c_name = "\033[1;49m"
      c_normal = "\033[0;49m"
    else
      c_bold = "\033[1;49m"
      c_name = "\033[1;34m"
      c_normal = "\033[0;49m"
    end

    if ENV['silent'] != "true" then
        sh "echo '#{c_bold}[ #{c_normal}#{c_name}#{ARTEFACT}#{c_normal}#{c_bold} ]#{c_normal}'"
    end
end

# These tasks allow projects to define actions that should happen before and
# after the build.
task :prebuild
task :postbuild

# The default rule that kicks of the build process
task :default => [:prebuild, :binary, :postbuild]

# For make junkies
task :all => :default

# Run test for project
task :test do
    verbose(false)
    begin
      sh "corto test"
    rescue
      abort
    end
end

# Rules for generated files
rule '_api.o' => ->(t){t.pathmap(".corto/%f").ext(".c")} do |task|
    build_source(task.source, task.name, false)
end
rule '_meta.o' => ->(t){t.pathmap(".corto/%f").ext(".c")} do |task|
    build_source(task.source, task.name, false)
end
rule '_wrapper.o' => ->(t){t.pathmap(".corto/%f").ext(".c")} do |task|
    build_source(task.source, task.name, false)
end
rule '_load.o' => ->(t){t.pathmap(".corto/%f").ext(".c")} do |task|
    build_source(task.source, task.name, false)
end

# Generic rule for translating source files into object files
rule '.o' => ->(t) {
    files = Rake::FileList["src/**/*.{c,cc,cpp,cxx}"]
    file = nil
    files.each do |e|
      base = File.join(File.dirname(t), File.basename(t, '.*'))
      if e.pathmap("%{^src/,.corto/obj/}X") == base then
          file = e
          break;
      end
    end
    if file == nil then
        file = t.pathmap("src/%f").ext(".c")
    end
    file
} do |task|
    build_source(task.source, task.name, true)
end

# --- UTILITIES

# Utility for building a sourcefile
def build_source(source, target, echo)
    verbose(false)
    flags = ""
    cc = ""

    # Can't use extname because of files with multiple periods in their name
    if source.split(".").last == "c" then
        flags = CFLAGS
        cc = CC
    else
        flags = CXXFLAGS
        cc = CXX
    end

    path = File.dirname(target)
    if not File.exists? path then
        sh "mkdir -p #{path}"
    end

    if echo
        if ENV['silent'] != "true" then
            sh "echo '#{source}'"
        end
    end

    cc_command = "#{cc} -c #{flags.join(" ")} #{DEFINE.map {|d| "-D" + d}.join(" ")} #{INCLUDE.map {|i| "-I" + i}.join(" ")} #{source} -o #{target}"
    begin
      sh cc_command
    rescue
      puts "\033[1;31mcommand failed: #{cc_command}\033[0;49m"
      abort()
    end
end
