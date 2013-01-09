require '_dealer'

c = Client.new "../test/hello_dealer.json"
#r = c.send("hello", "hello", "test")
t = {"deadline" => 2.0, "timeout" => 40.0, "urgent" => true, "persistent" => true, "max_retries" => 3.1}
s = "dsds"
r = c.send("hello", "hello", "test", t)
puts r.get(10)
puts r.get(10)
puts r.get(10)
puts r.get(10)
#puts r.get(30)
#for i in 0..1
#    r = c.send("hello", "hello", "test")
#    puts r.get(30)
#end

puts "Check old messages:"
puts c.get_msg_count("hello")
msgs = c.get_messages("hello")
puts msgs
for msg in msgs:
    r = c.send_manual(msg)
    puts r.get(10)
    c.remove_stored_message_for(r)
    c.remove_stored_message(msg)
end
c.get_msg_count("hello")
