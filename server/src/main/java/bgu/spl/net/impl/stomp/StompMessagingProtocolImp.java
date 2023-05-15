package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.ConnectionsImp;
import bgu.spl.net.srv.ConnectionHandler;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class StompMessagingProtocolImp implements StompMessagingProtocol<String> {
    
    private ConnectionsImp<String> connectim;
    private int connectionId;
    private boolean shouldTerminate;
    private AtomicInteger messageCounter;
        

    public StompMessagingProtocolImp(){
        shouldTerminate = false;
        messageCounter = new AtomicInteger(0);
    
    }

    public void start(int connectionId, Connections<String> connections){

        this.connectim = (ConnectionsImp<String>)connections;
        this.connectionId = connectionId;

    }
    
    public void process(String message){ // what is the process

        int typeIndex = message.indexOf('\n');
        String pType = message.substring(0, typeIndex);
        String body = message.substring(typeIndex+1);
        String sendBody = body;

        if(body != null && body.length() > 0){
            
            if(pType.equals("SEND")){
                int typeDes = sendBody.indexOf('\n');
                String destination = sendBody.substring(0, typeDes);
                // sendBody = sendBody.substring(typeDes+1);
                sendProcess(destination, sendBody);
            }
            else if(pType.equals("DISCONNECT")){

                String receipt= body.substring(body.indexOf("receipt:")+8, body.indexOf('\n')); // WE DIDNT DO WITH -id
                disconnectProcess(receipt);

            }
            else{
            
                ConcurrentHashMap<String, String> frameMap = textManage(body);
                


                if(pType.equals("CONNECT")){

                    connectProcess(frameMap);

                }
                // else if(pType.equals("SEND")){

                //     // sendProcess(frameMap, sendBody);
                // }
                else if(pType.equals("SUBSCRIBE")){

                    subscribeProcess(frameMap);
                
                }
                else if(pType.equals("UNSUBSCRIBE")){

                    unsubscribeProcess(frameMap);
                
                }
                else{ //ERROR - type incorrect
                    
                    String error = "ERROR\nmessage:command incorrect\n\n";
                    connectim.send(connectionId, error);
                    shouldTerminate = true;
                    //DISCONNECT

                }
            }
        
        }
        else{ // ERROR - body empty

            String error = "ERROR\nmessage:body is empty\n\n";
            connectim.send(connectionId, error);
            shouldTerminate = true;
            //DISCONNECT
        }

    } 

    public boolean shouldTerminate(){
        return shouldTerminate;
    }



    public void connectProcess(ConcurrentHashMap<String, String> frameMap){

        if(frameMap.get("accept-version").equals("1.2")){

            String state = connectim.userState(frameMap.get("login"), frameMap.get("passcode"));

            if(state.equals("RECONNECT")){

                connectim.reConnect(frameMap.get("login"), connectionId);
                connectim.send(connectionId, "CONNECTED\nversion:1.2\n\n");

            }
            else if(state.equals("NEW USER")){

                if(!connectim.viciousCheck(connectionId)){

                    connectim.newConnection(frameMap.get("login"), frameMap.get("passcode"), connectionId);
                    connectim.send(connectionId, "CONNECTED\nversion:1.2\n\n");
                }
                else{


                    String msg = "ERROR\n";
                    if(frameMap.get("receipt") != null){
                    
                        msg = msg+"receipt-id:"+frameMap.get("receipt")+"\n";

                    }
                    msg = msg+"message:You are already loggedin! you thought you got me haa! byeee\n\n";
                    connectim.send(connectionId, msg);
                    shouldTerminate = true;
                }


            }
            else if(state.equals("ALREADY CONNECTED")){ // ERROR



                String masg = "ERROR\n";
                if(frameMap.get("receipt") != null){
                
                    masg = masg+"receipt-id:"+frameMap.get("receipt")+"\n";

                }
                masg = masg+"message:The client is already logged in, log out before trying again\n\n";
                connectim.send(connectionId, masg);
                shouldTerminate = true;

                //DISCONNECT

            }
            else if(state.equals("WRONG PASSWORD")){ // ERROR
                

                String masg = "ERROR\n";
                if(frameMap.get("receipt") != null){
                
                    masg = masg+"receipt-id:"+frameMap.get("receipt")+"\n";

                }
                masg = masg+"message:Wrong password\n\n";
                connectim.send(connectionId, masg);
                shouldTerminate = true;

                //DISCONNECT

            }
            
        }
        else{ // ERROR

            String masg = "ERROR\n";
            if(frameMap.get("receipt") != null){
            
                masg = masg+"receipt-id:"+frameMap.get("receipt")+"\n";

            }
            masg = masg+"message:version incorrect\n\n";
            connectim.send(connectionId, masg);
            shouldTerminate = true;
            
            //DISCONNECT
        }

    }

    private void add(){

        int val;
        do{
            val = messageCounter.get();
        }while (!messageCounter.compareAndSet(val, val+1));
    }

    public void sendProcess(String destination, String message){

        // String channel = frameMap.get("destination");
        String channel = destination.substring(destination.indexOf(':')+1);

        if(channel != null && connectim.isTopicExists(channel)){

            if(connectim.isClientSubscribed(connectionId, channel)){

                ConcurrentLinkedQueue<ConnectionHandler<String>> connections = connectim.topicToClients.get(channel);
                for(ConnectionHandler<String> connect: connections){

                    int connecId = connectim.connectionToClient.get(connect);
                    String topId = connectim.topicToId.get(connecId).get(channel);

                    add(); // increasing the messageCounter
                    message = message.substring(0,message.indexOf("team a: ")) + "\n" + message.substring(message.indexOf("team a: "));
                    message = "MESSAGE\nsubscribtion:" + topId + "\nmessage-id:"+ messageCounter + "\n" + message;

                    connectim.send(connecId, message);
                }

            }
            else{ // ERROR
    
                
                String error = "ERROR\nmessage:clients not subscribed\n\n";
                connectim.send(connectionId, error);
                shouldTerminate = true;
                //DISCONNECT
    
            }
        }
        else{ // ERROR

            String error = "ERROR\nmessage:topic isnt exist\n\n";
            connectim.send(connectionId, error);
            shouldTerminate = true;
            //DISCONNECT
        }


    }

    public void subscribeProcess(ConcurrentHashMap<String, String> frameMap){

        String topic = frameMap.get("destination");
        String topicId = frameMap.get("id");
        String recieptId = frameMap.get("receipt");

        if(topic != null && topicId != null){

            connectim.addClientToTopic(connectionId, topicId, topic);

            if(recieptId != null){
                String mes = "RECEIPT\nreceipt-id:"+recieptId+"\n\n";
                connectim.send(connectionId, mes);
            }
        }
        else{ // ERROR

            String masg = "ERROR\n";
            if(frameMap.get("receipt") != null){
            
                masg = masg+"receipt-id:"+frameMap.get("receipt")+"\n";

            }
            masg = masg+"message:destination or id incorrect\n\n";
            connectim.send(connectionId, masg);
            shouldTerminate = true;

        }

    }

    public void unsubscribeProcess(ConcurrentHashMap<String, String> frameMap){

        String topicId = frameMap.get("id");
        String recieptId = frameMap.get("receipt");
                
        if(topicId != null){
            
            boolean err = connectim.removeClientFromTopic(connectionId, topicId);
            if(err){

                String msg = "ERROR\n";
                if(recieptId != null){
                    
                    msg = msg+"receipt-id:"+recieptId+"\n";

                }
                msg = msg+"message:client arnt subscribed to this destination\n\n";
                connectim.send(connectionId, msg);
                shouldTerminate = true;
            }
            else{

                if(recieptId != null){

                    String mes = "RECEIPT\nreceipt-id:"+recieptId+"\n\n";
                    connectim.send(connectionId, mes);
                }
            }
        }
        else{ //ERROR


            String msg = "ERROR\n";
                if(recieptId != null){
                    
                    msg = msg+"receipt-id:"+recieptId+"\n";

                }
                msg = msg+"message:message:id incorrect\n\n";
                connectim.send(connectionId, msg);

            shouldTerminate = true;

        }

    }

    public void disconnectProcess(String reciep){

        //String recieptId = frameMap.get("receipt");
        String recieptId = reciep;
        if(recieptId != null){

            connectim.disconnect(connectionId);
            String mes = "RECEIPT\nreceipt-id:"+recieptId+"\n\n";
            connectim.send(connectionId, mes);

            shouldTerminate = true;

        }
        else{ // ERROR

            String masg = "ERROR\n";
            masg = masg+"message:receipt incorrect\n\n";
            connectim.send(connectionId, masg);

            shouldTerminate = true;

        }

    }

    public ConcurrentHashMap<String, String> textManage(String body){

        ConcurrentHashMap<String, String> frameMap = new ConcurrentHashMap<>();

        int endLine;

        while(body.length() > 0){

            endLine = body.indexOf('\n');

            String line = body.substring(0, endLine);
            body = body.substring(endLine+1); // shorting the string
           
            String header = line.substring(0, line.indexOf(':'));
            String headerVal = line.substring(line.indexOf(':')+1, endLine);

            frameMap.put(header, headerVal);
            
            endLine = body.indexOf('\n'); // for the next iteration

        }

        return frameMap;

    }

}
