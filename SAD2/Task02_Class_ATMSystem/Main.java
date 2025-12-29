public class Main {

    public static void main(String[] args) {

        BankDatabase bank = BankDatabase.getInstance();

        bank.addAccount(new Account("1111", 1234, 5000));
        bank.addAccount(new Account("2222", 4321, 3000));

        ATM atm = new ATM();
        atm.start();
    }
}




