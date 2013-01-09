require "_dealer"
#require "msgpack"

class Dealer < Client

    def send(service, handle, message, *args )
        super(service, handle, message, *args)
    end

end


#==========================================================================
if __FILE__ == $0
    t = {"deadline" => 2.0, "timeout" => 40.0, "urgent" => true, "persistent" => true, "max_retries" => 3.1}
    d = Dealer.new("/home/noxiouz/git_repo/github/ruby/test/hello_dealer.json")
    r = d.send("hello", "hello", "PING!!!", t)
    puts r.get(10)
    r = d.send("hello", "hello", "PING!!!")
    puts r.get(10)
end
