require 'Client'

c = Client.new "test/hello_dealer.json"
#r = c.send("hello", "hello", "test")
t = {"deadline" => 2.0, "timeout" => 40.0, "urgent" => true, "persistent" => true}
s = "dsds"
r = c.send("hello", "hello", "test", t)
puts r.get(30)
#for i in 0..1
#    r = c.send("hello", "hello", "test")
#    puts r.get(30)
#end

puts c.get_msg_count("hello")
msgs = c.get_messages("hello")
puts msgs
for msg in msgs:
    c.remove_stored_message(msg)
end
c.get_msg_count("hello")
