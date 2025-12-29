import java.util.ArrayList;
import java.util.List;

public class BankDatabase {

    private static BankDatabase instance;
    private List<Account> accounts = new ArrayList<>();

    private BankDatabase() {

    }

    public static BankDatabase getInstance() {
        if (instance == null) {
            instance = new BankDatabase();
        }
        return instance;
    }
    public void addAccount(Account account) {
        accounts.add(account);
    }
    public void clearAccounts() {
        accounts.clear();
    }

    public boolean authenticate(String cardNumber, int pin) {
        for (Account acc : accounts) {
            if (acc.getCardNumber().equals(cardNumber)
                    && acc.validatePin(pin)) {
                return true;
            }
        }
        return false;
    }

    public double getBalance(String cardNumber) {
        for (Account acc : accounts) {
            if (acc.getCardNumber().equals(cardNumber)) {
                return acc.getBalance();
            }
        }
        return 0;
    }

    public boolean withdraw(String cardNumber, double amount) {
        for (Account acc : accounts) {
            if (acc.getCardNumber().equals(cardNumber)) {
                return acc.withdraw(amount);
            }
        }
        return false;
    }
}
