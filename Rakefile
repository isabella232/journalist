require "bundler/gem_tasks"

require "rake/extensiontask"

Rake::ExtensionTask.new("journalist") do |ext|
  ext.lib_dir = "lib/journalist"
end

task default: [:clobber, :compile]
