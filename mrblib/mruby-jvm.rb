def __main__(argv)
  command = argv.shift
  java_class = argv.shift
  java_opts = []
  while argv.first != "--" and !argv.empty?
    java_opts << argv.shift
  end
  argv.shift
  JavaSupport.exec_java(java_class, java_opts, argv)
end
