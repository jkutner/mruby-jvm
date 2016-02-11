class JavaSupport

  attr_reader :java_home
  attr_reader :java_exe
  attr_reader :runtime
  attr_reader :java_server_dl
  attr_reader :java_client_dl

  def self.exec_java(java_opts, java_class, program_opts)
    new.exec_java(java_opts, java_class, program_opts)
  end

  def self.system_java(java_opts, java_class=nil, program_opts=[])
    new.system_java(java_opts, java_class, program_opts)
  end

  def self.custom_java(java_home, java_opts, java_class=nil, program_opts=[])
    new(java_home).system_java(java_opts, java_class, program_opts)
  end

  def initialize(java_home=nil)
    @runtime, @java_exe, @java_server_dl, @java_client_dl, @java_home = resolve_java_home(java_home)
  end

  def resolve_java_home(java_home)
    info = attempt_javacmd(ENV['JAVACMD']) ||
           attempt_java_home(java_home || ENV['JAVA_HOME']) ||
           resolve_native_java_home
    raise "No JAVA_HOME found." unless info
    info
  end

  def resolve_native_java_home
    native_java_home = find_native_java
    return nil unless native_java_home
    native_java_home.strip!
    if native_java_home.end_with?("/bin/java")
      native_java_home = File.expand_path("../..", native_java_home)
    end
    attempt_java_home(native_java_home)
  end

  def attempt_javacmd(javacmd)
    return nil unless javacmd

    java_bin = File.dirname(javacmd)
    attempt_java_home(File.dirname(java_bin), javacmd)
  end

  def attempt_java_home(path, javacmd=nil)
    return nil if path.nil?
    path.strip!
    return nil unless Dir.exists?(path)

    try_jdk_home(path, javacmd) ||
      try_jre_home(path, javacmd) ||
      try_jdk9_home(path, javacmd)
  end

  def try_jdk_home(path, javacmd=nil)
    exe = exists_or_nil(resolve_java_exe(path))
    sdl = exists_or_nil(resolve_jdk_server_dl(path)) || exists_or_nil(resolve_jamvm_dl(path))
    cdl = exists_or_nil(resolve_jdk_client_dl(path))
    return nil unless exe and (cdl or sdl)
    [:jdk, javacmd || exe, sdl, cdl, path]
  end

  def try_jdk9_home(path, javacmd=nil)
    exe = exists_or_nil(resolve_java_exe(path))
    sdl = exists_or_nil(resolve_jdk9_server_dl(path))
    return nil unless exe and sdl
    [:jdk9, javacmd || exe, sdl, nil, path]
  end

  def try_jre_home(path, javacmd=nil)
    exe = exists_or_nil(resolve_java_exe(path))
    sdl = exists_or_nil(resolve_jre_server_dl(path))
    cdl = exists_or_nil(resolve_jre_client_dl(path))
    return nil unless exe and (cdl or sdl)
    [:jre, javacmd || exe, sdl, cdl, path]
  end

  def resolve_java_exe(java_home)
    File.join(java_home, "bin", JavaSupport::JAVA_EXE)
  end

  def resolve_jdk_server_dl(java_home)
    File.join(java_home, "jre", JavaSupport::JAVA_SERVER_DL)
  end

  def resolve_jdk9_server_dl(java_home)
    File.join(java_home, JavaSupport::JAVA_SERVER_DL)
  end

  def resolve_jdk_client_dl(java_home)
    File.join(java_home, "jre", JavaSupport::JAVA_CLIENT_DL)
  end

  def resolve_jre_client_dl(java_home)
    File.join(java_home, JavaSupport::JAVA_CLIENT_DL)
  end

  def resolve_jre_server_dl(java_home)
    File.join(java_home, JavaSupport::JAVA_SERVER_DL)
  end

  def resolve_jamvm_dl(java_home)
    File.join(java_home, "lib/jamvm/libjvm.so")
  end

  def resolve_jli_dl
    if @runtime == :jdk
      File.join(@java_home, "jre", JavaSupport::JLI_DL)
    else
      File.join(@java_home, JavaSupport::JLI_DL)
    end
  end

  def resolve_java_dls(java_opts)
    client_i = java_opts.index("-client")
    server_i = java_opts.index("-server")
    if client_i.nil? && server_i.nil?
      java_dl = @java_server_dl || @java_client_dl
    elsif server_i
      java_dl = @java_server_dl
    elsif client_i
      java_dl = @java_client_dl
    elsif server_i < client_i
      java_dl = @java_client_dl
    else
      java_dl = @java_server_dl
    end

    raise "Could not find Java native library" if java_dl.nil?

    yield java_opts.select{|o| !["-client","-server"].include?(o) }, java_dl, resolve_jli_dl
  end

  def exec_java(java_opts, java_class, program_opts)
    cmd_line_prop = "-Dsun.java.command=#{java_class}"
    resolve_java_dls([cmd_line_prop] + java_opts) do |parsed_java_opts, java_dl, jli_dl|
      all_opts = parsed_java_opts + program_opts
      _exec_java_ @java_exe, java_dl, jli_dl, java_class, parsed_java_opts.size, *all_opts
    end
  end

  def system_java(java_opts, java_class=nil, program_opts=[])
    all_opts = java_opts + program_opts
    _system_java_ @java_exe, nil, nil, java_class, java_opts.size, *all_opts
  end

  def self.cp_delim
    CLASSPATH_DELIMITER
  end

  private

  def exists_or_nil(path)
    File.exists?(path) ? path : nil
  end
end
