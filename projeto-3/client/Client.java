package client;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.Scanner;
import compute.Compute;
import java.util.concurrent.TimeUnit;
import java.io.PrintWriter;
import java.io.FileWriter;

public class Client {

    public static void main(String args[]) {
	     String request;

        //if (System.getSecurityManager() == null) {
        //    System.setSecurityManager(new SecurityManager());
        //}
        try {
            String name = "Compute";
            Registry registry = LocateRegistry.getRegistry(args[0]);
            Compute comp = (Compute) registry.lookup(name);

            // Print help
            System.out.println(comp.executeRequest("help"));

            Scanner scanner = new Scanner(System.in);

            while(scanner.hasNextLine()){

                // Read client input
                request = scanner.nextLine();

                // Exit if requested
                if(request.equals("exit")){
                    break;
                }

                // Send request to server and print response
                long startTime = System.nanoTime();
                String response = comp.executeRequest(request);
                long endTime = System.nanoTime();
                long timeElapsed = endTime - startTime;

                System.out.println("-----------------------------------------");
                System.out.println(response);

                // Print and save time
                System.out.println("<<Execution time in nanoseconds: " + timeElapsed + ">>");
                if (args.length > 1) {
                    FileWriter fileWriter = new FileWriter(args[1], true);
                    PrintWriter printWriter = new PrintWriter(fileWriter);
                    printWriter.print(timeElapsed + "\n");
                    printWriter.close();
                }
                System.out.println("-----------------------------------------");
            }

        } catch (Exception e) {
            System.err.println("Client exception:");
            e.printStackTrace();
        }
    }
}
