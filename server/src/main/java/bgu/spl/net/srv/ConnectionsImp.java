package bgu.spl.net.srv;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

import java.util.concurrent.atomic.AtomicInteger;

public class ConnectionsImp<T> implements Connections<T>{

    private ConcurrentHashMap<String, String> usePassMap; // user -> password
    private ConcurrentHashMap<String, Boolean> isLoggedMap; // user -> isLogged
    private ConcurrentHashMap<Integer, ConcurrentHashMap<String, String>> idToTopic; // connectionId -> topicId -> topic
    public ConcurrentHashMap<Integer, ConcurrentHashMap<String, String>> topicToId; // connectionId -> topic -> topicId
    public ConcurrentHashMap<String, ConcurrentLinkedQueue<ConnectionHandler<T>>> topicToClients; // topic -> connectionHandlers
    public ConcurrentHashMap<Integer, ConnectionHandler<T>> clientToConnection;
    public ConcurrentHashMap<ConnectionHandler<T>, Integer> connectionToClient;
    private ConcurrentHashMap<Integer, String> connectionIdToName; // connectionId -> login
    
    private AtomicInteger connectionIdCounter; //ATOMICCCCCCC
    
    public ConnectionsImp(){

        usePassMap = new ConcurrentHashMap<>();
        isLoggedMap= new ConcurrentHashMap<>();
        connectionIdCounter = new AtomicInteger(0);
        
        idToTopic = new ConcurrentHashMap<>();
        topicToId = new ConcurrentHashMap<>();
        topicToClients = new ConcurrentHashMap<>();
        clientToConnection = new ConcurrentHashMap<>();
        connectionToClient = new ConcurrentHashMap<>();
        connectionIdToName = new ConcurrentHashMap<>();

    }

    public boolean viciousCheck(Integer conId){ // checks if user trying to login when logged in

        
        boolean ans = connectionIdToName.containsKey(conId);
        return ans;

    }

    private void add(){

        int val;
        do{
            val = connectionIdCounter.get();
        }while (!connectionIdCounter.compareAndSet(val, val+1));
    }

    public boolean send(int connectionId, T msg){

        if(clientToConnection.containsKey(connectionId)){

            clientToConnection.get(connectionId).send(msg);
            return true;
        }
        return false;

    }

    public void send(String channel, T msg){

        if(topicToClients.contains(channel)){

            for(ConnectionHandler<T> ch: topicToClients.get(channel)){
                
                ch.send(msg);
            }        
        }
        else{
            System.out.println("ERROR");
        }
    }

    public void disconnect(int connectionId){

        isLoggedMap.replace(connectionIdToName.get(connectionId), false);

        
        ConcurrentHashMap<String, String> topics = idToTopic.get(connectionId);

        if(topics != null){ // I ADDED THE IF
            
            for(String topicId: topics.keySet()){

                String topic = topics.get(topicId);
                topicToClients.get(topic).remove(clientToConnection.get(connectionId));
    
            }
        }
        
        
        idToTopic.remove(connectionId); // I REMOVED FROM THE MAP
        connectionIdToName.remove(connectionId);

    }


    public void reConnect(String login, int connectionId){

        isLoggedMap.replace(login, true);
        connectionIdToName.put(connectionId, login);

        ConcurrentHashMap<String, String> toIdTopic = new ConcurrentHashMap<>();
        ConcurrentHashMap<String, String> toTopicId = new ConcurrentHashMap<>();
        idToTopic.put(connectionId, toIdTopic);
        topicToId.put(connectionId, toTopicId);

        // ConcurrentHashMap<String, String> topics = idToTopic.get(connectionId);

        // for(String topicId: topics.keySet()){

        //     String topic = topics.get(topicId);
        //     topicToClients.get(topic).add(clientToConnection.get(connectionId));
        // }


    }

    public void newConnection(String login, String pass, int connectionId){

        usePassMap.put(login, pass);
        isLoggedMap.put(login, true);
        connectionIdToName.put(connectionId, login);
        
        ConcurrentHashMap<String, String> toIdTopic = new ConcurrentHashMap<>();
        ConcurrentHashMap<String, String> toTopicId = new ConcurrentHashMap<>();
        idToTopic.put(connectionId, toIdTopic);
        topicToId.put(connectionId, toTopicId);
    }

    public void addClientConnectionHandler(int connectionId, ConnectionHandler<T> ch){ // WE NEED TO CALL IT FROM BLOCKING OR NON BLOCKING

        clientToConnection.put(connectionId, ch);
        connectionToClient.put(ch, connectionId);

    }

    public boolean isClientSubscribed(int connectionId, String topic){

        boolean ans = false;

        if(topicToClients.get(topic).contains(clientToConnection.get(connectionId))){
            ans = true;
        }

        return ans;

    }

    public boolean isTopicExists(String topic){
        
        return topicToClients.containsKey(topic);
    }

    public String userState(String login, String pass){

        String ans;
        
        if(usePassMap.containsKey(login)){ // the user exists

            if(usePassMap.get(login).equals(pass)){ //the password is correct

                if(isLoggedMap.get(login)){ // already connected

                    ans = "ALREADY CONNECTED";
                }
                else{ // regular connect

                    ans = "RECONNECT";
                }

            }
            else{ // the password is incorrect

                ans = "WRONG PASSWORD";
            }
        }
        else{ // its a new user

            ans = "NEW USER";

        }
        return ans;
    }

    public int getConnectionId(){  

        add();
        return connectionIdCounter.get();
    }

    public void addClientToTopic(int connectionId, String topicId, String topic){

        if(topicToClients.contains(topic)){

            topicToClients.get(topic).add(clientToConnection.get(connectionId));
            idToTopic.get(connectionId).put(topicId, topic);
            topicToId.get(connectionId).put(topic, topicId);
            
        }
        else{

            ConcurrentLinkedQueue<ConnectionHandler<T>> temp = new ConcurrentLinkedQueue<>();
            temp.add(clientToConnection.get(connectionId));
            topicToClients.put(topic, temp);

            idToTopic.get(connectionId).put(topicId, topic);
            topicToId.get(connectionId).put(topic, topicId);
        }

    }

    public boolean removeClientFromTopic(int connectionId, String topicId){

        if(idToTopic.get(connectionId).containsKey(topicId)){
            
            String topic = idToTopic.get(connectionId).get(topicId);
            topicToClients.get(topic).remove(clientToConnection.get(connectionId)); // removes
            idToTopic.get(connectionId).remove(topicId); // removes

            topicToId.get(connectionId).remove(topic);

            return false;

        }
        else{ // ERROR
            return true;
        }

    }

}
