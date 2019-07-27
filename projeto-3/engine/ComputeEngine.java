package engine;

import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.io.*;
import java.util.HashMap;
import java.lang.*;
import compute.Compute;
import engine.Profile;
import java.util.concurrent.TimeUnit;

public class ComputeEngine implements Compute {

    public HashMap<String,Profile> map = null;

    public ComputeEngine() {

        // Call executeRequest()
        super();
    }

    public String executeRequest(String request) {
        String response = "";

        long startTime = System.nanoTime();

        // De-serialization
        try {
            FileInputStream fis = new FileInputStream("data.ser");
            ObjectInputStream ois = new ObjectInputStream(fis);
            map = (HashMap) ois.readObject();
            ois.close();
            fis.close();
        } catch(IOException ioe) {
            ioe.printStackTrace();
            return "Reading database error";
        } catch(ClassNotFoundException cnf) {
    	      return "Class not found";
    	  }


        try {

            // Split request into type and data
            String[] input = request.split(" ", 2);

            // Send help information
            if (input[0].equals("help")){
                return "Opções disponíveis: \n" +
                       "1 - listar todas as pessoas formadas em um determinado curso; \n" +
                       "2 - listar as habilidades dos perfis que moram em uma determinada cidade; \n" +
                       "3 - acrescentar uma nova experiência em um perfil; \n" +
                       "4 - dado o email do perfil, retornar sua experiência; \n" +
                       "5 - listar todas as informações de todos os perfis; \n" +
                       "6 - dado o email de um perfil, retornar suas informações.\n" +
                       "------------------------------------------------------------------------------";
            }

            // Select request type
            switch(Integer.parseInt(input[0])) {
                case 1:
                    //System.out.println("retrieving name by course...");
                    response = namesByCourse(input[1]);
                    //System.out.println("names retrieved");
                    break;
                case 2:
                    //System.out.println("retrieving habilities by city...");
                    response = habilitiesByCity(input[1]);
                    //System.out.println("habilities retrieved");
                    break;
                case 3:
                    //System.out.println("adding experience...");
                    response = addExperience(input[1]);
                    break;
                case 4:
                    //System.out.println("retrieving experiences...");
                    response = getExperience(input[1]);
                    //System.out.println("experiences retrieved");
                    break;
                case 5:
                    //System.out.println("sending all profiles...");
                    response = getAllProfiles();
                    //System.out.println("all profiles sent");
                    break;
                case 6:
                    //System.out.println("retrieving profile...");
                    response = getProfile(input[1]);
                    //System.out.println("profile sent");
                    break;
                default:
                    //System.out.println("Invalid option");
                    response = "Invalid option";
            }
        } catch(NumberFormatException nfe) {
            return "Invalid option: " + nfe.getMessage();
        }

        // Serialization
        try {
            FileOutputStream fos =
            new FileOutputStream("data.ser");
            ObjectOutputStream oos = new ObjectOutputStream(fos);
            oos.writeObject(map);
            oos.close();
            fos.close();
        } catch(IOException ioe) {
            ioe.printStackTrace();
            return "Saving database error";
        }

        long endTime = System.nanoTime();
        long timeElapsed = endTime - startTime;
        System.out.println(timeElapsed);

        return response;
    }

    public String namesByCourse(String course) {
        String names = "";

        for (Profile p : map.values()) {
            if (p.getFormacao().equals(course)) {
                names += p.getNomeCompleto() + "\n";
            }
        }

        return names;
    }

    public String habilitiesByCity(String city) {
        String habilities = "";

        for (Profile p : map.values()) {
            if (p.getResidencia().equals(city)) {
                habilities += p.getNomeCompleto() + ": " + p.getHabilidades() + "\n";
            }
        }

        return habilities;
    }

    public String addExperience(String data) {
        String response;

        String[] token = data.split(" ", 2); // email and experience

        if (map.containsKey(token[0])) {
            Profile p = map.get(token[0]);
            p.addExperiencia(token[1]); // Verify if its really saved into the map
            response = "Experience added";
        }
        else {
            response = "Profile not found";
        }

        return response;
    }

    public String getExperience(String email) {
        String response;

        if (map.containsKey(email)) {
            Profile p = map.get(email);
            response = p.getExperiencias();
        }
        else {
            response = "Profile not found";
        }

        return response;
    }

    public String getAllProfiles() {
        String response = "";

        for (Profile p : map.values()) {
            response += p.toString();
        }

        return response;
    }

    public String getProfile(String data) {
        String response;

        if (map.containsKey(data)) {
            Profile p = map.get(data);
            response = p.toString();
        }
        else {
            response = "Profile not found";
        }

        return response;
    }

    public static void main(String[] args) {
        //if (System.getSecurityManager() == null) {
        //    System.setSecurityManager(new SecurityManager());
        //}
        try {
            String name = "Compute";
            Compute engine = new ComputeEngine();
            Compute stub = (Compute) UnicastRemoteObject.exportObject(engine, 0);
            Registry registry = LocateRegistry.getRegistry();
            registry.rebind(name, stub);
            //System.out.println("ComputeEngine bound");
        } catch (Exception e) {
            System.err.println("ComputeEngine exception:");
            e.printStackTrace();
        }
    }
}
