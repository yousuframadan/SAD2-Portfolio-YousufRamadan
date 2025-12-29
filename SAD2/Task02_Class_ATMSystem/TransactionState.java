public class TransactionState implements ATMState {

    @Override
    public void handle(ATM atm) {
        atm.getScreen().displayMessage("Choose transaction:");
        atm.getScreen().displayMessage("1 - Withdraw");
        atm.getScreen().displayMessage("2 - Balance Inquiry");

        int choice = atm.getKeypad().getInput();

        switch (choice) {
            case 1 -> atm.setState(new WithdrawState());
            case 2 -> {
                double bal = atm.getBankDatabase()
                        .getBalance(atm.getCurrentCardNumber());
                atm.getScreen().displayMessage("Your balance is: " + bal);
                return;
            }
            default -> {
                atm.getScreen().displayMessage("Invalid choice");
                return;
            }
        }
        atm.getState().handle(atm);
    }
}
