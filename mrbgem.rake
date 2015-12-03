MRuby::Gem::Specification.new('mruby-jvm') do |spec|
  spec.license = 'MIT'
  spec.author  = 'Joe Kutner'
  spec.summary = 'mruby-jvm'
  spec.bins    = ['mruby-jvm']

  spec.add_test_dependency 'mruby-mtest', :mgem => 'mruby-mtest'
  spec.add_dependency 'mruby-env', :mgem => 'mruby-env'
  spec.add_dependency 'mruby-dir', :mgem => 'mruby-dir'
  spec.add_dependency 'mruby-string-ext', :core => 'mruby-string-ext'
  spec.add_dependency 'mruby-io', :mgem => 'mruby-io'

  spec.cc.include_paths << "#{spec.build.gem_clone_dir}/mruby-io/include"
end
