package bgu.spl.net.impl.stomp;
import bgu.spl.net.srv.Reactor;
import bgu.spl.net.srv.Server;
import bgu.spl.net.srv.ConnectionsImp;

public class StompServer {

    public static void main(String[] args) {
        // TODO: implement this

        ConnectionsImp<String> connectim = new ConnectionsImp<>();
        if(args[1].equals("tpc")){

            Server.threadPerClient(Integer.decode("7777").intValue(),
             ()-> new StompMessagingProtocolImp(), ()-> new MessageEncoderDecoderImp(), connectim).serve();

        }
        else if(args[1].equals("reactor")){
            Reactor<String> server = new Reactor<>(3, Integer.decode(args[0]).intValue(),
             ()-> new StompMessagingProtocolImp(), ()-> new MessageEncoderDecoderImp(), connectim);
            server.serve();

        }
    }
}
