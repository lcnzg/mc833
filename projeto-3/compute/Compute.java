package compute;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface Compute extends Remote {
    String executeRequest(String request) throws RemoteException;
}
