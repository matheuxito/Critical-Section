public class TrafficController {
    private boolean isCarOnBridge = false;

    public synchronized void enterLeft() throws InterruptedException {
        while (isCarOnBridge) {
            wait();
        }
        isCarOnBridge = true;
    }

    public synchronized void enterRight() throws InterruptedException {
        while (isCarOnBridge) {
            wait();
        }
        isCarOnBridge = true;
    }

    public synchronized void leaveLeft() {
        isCarOnBridge = false;
        notifyAll();
    }

    public synchronized void leaveRight() {
        isCarOnBridge = false;
        notifyAll();
    }
}
