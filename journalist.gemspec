# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'journalist/version'

Gem::Specification.new do |spec|
  spec.name          = "journalist"
  spec.version       = Journalist::VERSION
  spec.authors       = ["Andre Medeiros"]
  spec.email         = ["me@andremedeiros.info"]
  spec.extensions    = ["ext/journalist/extconf.rb"]
  spec.summary       = %q{Easily look into a running Ruby process.}
  spec.description   = <<-DESC.gsub(/^\s|\t|\n/, '').lstrip
    Journalist is a gem that intends to make it really easy to get visibility
    into any running Ruby processes with metrics like GC stats, performance and
    allocations.
  DESC
  spec.homepage      = "https://github.com/journalistrb/journalist"
  spec.license       = "MIT"

  spec.files         = `git ls-files -z`.split("\x0")
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib"]

  spec.add_development_dependency "bundler", "~> 1.7"
  spec.add_development_dependency "rake", "~> 10.0"
  spec.add_development_dependency "rake-compiler"
end
