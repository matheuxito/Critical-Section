import java.util.concurrent.Semaphore;

public class TrafficController {
   private final Semaphore semaphore = new Semaphore(1); 
    public void enterLeft() {
        
        try {
            semaphore.acquire();
        } catch (InterruptedException e) {
            System.out.println("InterruptedException - esquerda");
        }
    }
    public void enterRight() {
        try {
            semaphore.acquire();
        } catch (InterruptedException e) {
            System.out.println("InterruptedException - direita");

        }
    }
    public void leaveLeft() {
        semaphore.release();
    }
    public void leaveRight() {
        semaphore.release();
    }
}
