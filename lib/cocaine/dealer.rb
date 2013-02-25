require "rubygems"
require "msgpack"

require "_dealer"

class Dealer < Client

    def send(service, handle, message, *args )
        if !message.instance_of? String
            message = MessagePack.pack(message)
        end
        super(service, handle, message, *args)
    end

end


#==========================================================================
if __FILE__ == $0
    t = {"deadline" => 10.0, "timeout" => -1, "urgent" => true, "persistent" => true, "max_retries" => -1}
    d = Dealer.new("/home/noxiouz/git_repo/github/ruby/test/hello_dealer.json")
    r = d.send("hello", "hello", "PING!!!", t)
    puts r.get(10)
    r = d.send("hello", "hello", 1)
    puts r.get(10)
end
