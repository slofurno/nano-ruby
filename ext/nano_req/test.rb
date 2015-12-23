#!/usr/local/bin/ruby

require './nanoreq'
require 'thread'

threads = []

["steve", "esteban"].each do |name|
    threads << Thread.new { 
        req = NanoReq.new("tcp://127.0.0.1:3434")
        
        req.send("#{name} says YO")
        puts "#{name} heard #{req.recv()}"
        req.send("#{name} says YO")
        puts "#{name} heard #{req.recv()}"
        req.send("#{name} says YO")
        puts "#{name} heard #{req.recv()}"
        req.send("#{name} says YO")
        puts "#{name} heard #{req.recv()}"
        req.send("#{name} says YO")
        puts "#{name} heard #{req.recv()}"
        req.close
    }
end

threads.each { |t| t.join }
