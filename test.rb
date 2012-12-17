require 'Client'

c = Client.new "test/hello_dealer.json"
r = c.send("hello", "hello", "test")
r.get(30)
for i in 0..50
    r = c.send("hello", "hello", "test")
    puts r.get(30)
end
