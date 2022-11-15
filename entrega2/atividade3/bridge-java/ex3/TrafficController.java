import java.util.concurrent.*;

public class TrafficController {
    private Semaphore mutex = new Semaphore(1, true);

    public void enterLeft() {
        try {
            mutex.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void enterRight() {
        try {
            mutex.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void leaveLeft() {
        mutex.release();
    }

    public void leaveRight() {
        mutex.release();
    }

}
