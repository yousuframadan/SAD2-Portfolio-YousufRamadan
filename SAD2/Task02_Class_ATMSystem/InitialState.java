public class InitialState implements ATMState {

    @Override
    public void handle(ATM atm) {
        atm.getScreen().displayMessage("Please insert your card");
        ATMCard card = atm.getCardReader().readCard();

        if (card != null) {
            atm.setCurrentCardNumber(card.getCardNumber());
            atm.getScreen().displayMessage("Card detected");
            atm.setState(new PinState());
            atm.getState().handle(atm);
        } else {
            atm.getScreen().displayMessage("No card detected");
        }
    }
}
