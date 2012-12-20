require 'Client'

c = Client.new "test/hello_dealer.json"
r = c.send("hello", "hello", "test")
r.get(30)
for i in 0..1
    r = c.send("hello", "hello", "test")
    puts r.get(30)
end

c.get_msg_count("hello")
msg = c.get_messages("hello")
