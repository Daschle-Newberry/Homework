import readline from "readline-sync";
import { Deck,Card } from "./deck";
import { Player, Dealer } from "./player";
import * as Util from "./util";
// *--------UTILITY--------*

class Game {
	private deck: Deck;
	private player: Player;
	private dealer: Dealer;

	public constructor() {
		this.deck = new Deck();
		this.deck.shuffle();

		this.player = new Player("Jim");
		this.dealer = new Dealer();
	}

	public start(): void {
		const bet: number = readline.questionInt(`${this.player.name} place your bet: `);
		this.player.setBet(bet);

		this.dealCards(this.player, 2);
		this.dealCards(this.dealer, 2);

		this.dealer.showHandConcealed();
		this.player.showHand();

		while(this.player.getPoints() < 21) {
			const decision: string = readline.question("(h)it or (s)tay? ");
			console.log("");
			if(decision === "s") break;
			
			this.dealCards(this.player, 1);
			this.player.showHand();
		}

		if(this.player.getPoints() > 21){
			console.log("Dealer wins!");
		}else{
			while(this.dealer.getPoints() < 17) {
					this.dealCards(this.dealer, 1);
			}
			this.dealer.showHand();
			if(this.dealer.getPoints() > 21 || this.player.getPoints() > this.dealer.getPoints()) console.log("Player wins!");
			else if(this.dealer.getPoints() > this.player.getPoints()) console.log("Dealer wins!");
			else console.log("Push!");
		}
	}

	private dealCards(player: Player, count: number): void {
		for(let i = 0; i < count; i++) {
			player.dealCard(this.deck.getCard());
		}
	}
}
const game: Game = new Game();
game.start();
