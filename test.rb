require 'Client'

c = Client.new "hello_dealer.json"
r = c.send("hello", "hello", "test")
r.get(30)
r = c.send("hello", "hello", "test")
r.get(30)
r = c.send("hello", "hello", "test")
r.get(30)

