public class CashDispenser {

    private int cashAvailable = 10000;

    public boolean dispenseCash(int amount) {
        if (amount <= cashAvailable) {
            cashAvailable -= amount;
            return true;
        }
        return false;
    }
}
