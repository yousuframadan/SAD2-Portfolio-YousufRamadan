public class PinState implements ATMState {

    @Override
    public void handle(ATM atm) {
        atm.getScreen().displayMessage("Enter your PIN:");
        int pin = atm.getKeypad().getInput();

        boolean authenticated = atm.getBankDatabase()
                .authenticate(atm.getCurrentCardNumber(), pin);

        if (authenticated) {
            atm.getScreen().displayMessage("Authentication successful");
            atm.setState(new TransactionState());
            atm.getState().handle(atm);
        } else {
            atm.getScreen().displayMessage("Authentication failed");
        }
    }
}
