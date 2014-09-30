# Journalist

A gem that allows you to look into a running Ruby process.

## Motivation

We lack the tools to have real time visibility in Ruby processes. After seeing [VisualVM](http://visualvm.java.net/) at Baruco, I've realised that a tool like that for CRuby would be amazing. Thus, Journalist was born.

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'journalist'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install journalist

## Usage

To start profiling, type:

    Journalist.start

There are a lot of limitations right now, such as this only working for one process at any given time.

## TODO

- [X] Finish call/return tracepoints
- [X] Implement allocation tracepoints
- [X] Implement event aggregation
- [X] Implement JSON serialization
- [ ] Implement command and stream sockets
- [ ] Build GUI

## Contributing

1. Fork it ( https://github.com/journalistrb/journalist/fork )
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request
