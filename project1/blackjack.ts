import readline from "readline-sync";
import { Renderer } from "./renderer";
import * as Util from "./util";

class Card {
	public suit: string;
	public type: string;
	public value: number;

	public constructor(suit: string, type: string, value: number) {
		this.suit = suit;
		this.type = type;
		this.value = value;
	}

	public frontFace(): string {
		// Ascii art heavily inspired by https://www.asciiart.eu/art/2eb26a8e809d26a8
		const numSpaces = 4 - (this.type.length - 1);
		return 	` _____ \n`+
				`|${this.type}${" ".repeat(numSpaces)}|\n`+
				`|     |\n`+
				`|  ${this.suit}  |\n`+
				`|     |\n`+
				`|${" ".repeat(numSpaces)}${this.type}|\n`+
				` ----- `;
	}

	public backFace(): string {
		return  ` _____ \n`+
				`|*~~~*|\n`+
				`|~   ~|\n`+
				`|~   ~|\n`+
				`|~   ~|\n`+
				`|*~~~*|\n`+
				` ----- `;
	}
}				

class Deck {
	private cards: Card[] = [];

 	public constructor() {
 		const suits: string[] = ["H","S","C","D"];
 		const types: string[] = ["2","3","4","5","6","7","8","9","10","J","Q","K", "A"];
 		for (const s of suits) { 
 			for(const t of types) {
 				let value: number;
 				if(t === "J" || t === "Q" || t === "K") value = 10;
 				else if(t === "A") value = 11;
 				else value = parseInt(t,10);
 				this.cards.push(new Card(s,t,value));
 			}
 		}
 	}

 	public getCard(): Card {
 		const card: Card | undefined = this.cards.shift();

 		if(!card) throw new Error("Illegal Game State: Deck empty");
 		
 		return card;
 	}

 	public shuffle(): void { 
 		// Fisher-Yates shuffle
 		let index = this.cards.length - 1;
 		while(index > 0) {
 			const swapIndex: number = Math.floor(Math.random() * (index + 1));
 			const tmp: Card = this.cards[index];
 			this.cards[index] = this.cards[swapIndex];
 			this.cards[swapIndex] = tmp;
 			index--;
 		}
 	}
}

class Player {
	public name: string;
	protected bet: number = 0;
	protected hand: Card[] = [];

	public constructor(name: string) {
		this.name = name;
	}

	public setBet(bet: number): void {
		this.bet = bet;
	}

	public hitStay(): boolean{
		if(this.getPoints() >= 21) return false;
		return "h" === readline.question("(h)it or (s)tay? ");
	}

	public getHandAscii(): string {
		if(this.hand.length < 1) return "";

		const handString: string[] = [];
		for(const card of this.hand) {
			handString.push(card.frontFace());
		}
		
		return Util.concatByLine(handString) + `Points: ${this.getPoints()}\n`;
	}

	public dealCard(card: Card): void { 
		this.hand.push(card);
	}

	public getPoints(): number {
		let points: number = 0;
		let aces = 0;
		for(const c of this.hand) {
			points += c.value;
			if(c.type === "A") aces++;
		}

		const requiredAces =  Math.max(Math.ceil((points/10) - 2.1), 0);
		return points - Math.min(requiredAces,aces) * 10;
	}
}

class Dealer extends Player {
	constructor() {
		super("Dealer");
	}

	// Override
	public hitStay(): boolean{
		return this.getPoints() < 17;
	}

	public getConcealedHandAscii(): string {
		if(this.hand.length < 1) return "";
		
		const handString: string[] = [];
		handString.push(this.hand[0].backFace());

		for(const card of this.hand.slice(1)) {
			handString.push(card.frontFace());
		}
		
		return Util.concatByLine(handString) + `Points: ${this.getPointsConcealed()} + ?\n`;
	}

	private getPointsConcealed(): number {
		let points = 0;
		let aceCount = 0;
	
		for(const c of this.hand.slice(1)) {
			if(c.type === "A") aceCount++;
			points += c.value;
		}
		const requiredAces =  Math.max(Math.ceil((points/10) - 2.1), 0);
		if(requiredAces > aceCount) return points;
		return points - requiredAces * 10;
	}
}

interface IState {
	update(): void;
	render(renderer: Renderer): void;
}

enum PlayState {
	PLAYER_TURN,
	DEALER_TURN,
	ROUND_OVER,
}
class Playing implements IState{
	private deck: Deck;
	private player: Player;
	private dealer: Dealer;
	private playState: PlayState;

	constructor() {
		this.deck = new Deck();
		this.deck.shuffle();

		this.player = new Player("Jim");
		this.dealer = new Dealer();
		
		this.playState = PlayState.PLAYER_TURN;

		this.dealCards(this.player, 2);
		this.dealCards(this.dealer, 2);
	}

	update() {
		switch (this.playState){
			case PlayState.PLAYER_TURN:
				if(this.player.hitStay()){
					this.dealCards(this.player,1);
				}else{
					this.playState = PlayState.DEALER_TURN;
				}
				break;
			case PlayState.DEALER_TURN:
				if(this.dealer.hitStay()){
					this.dealCards(this.dealer,1);
				}else{
					this.playState = PlayState.ROUND_OVER;
				}
				break;
			case PlayState.ROUND_OVER:
				game.exitState();
				break;
		}
		
	}

	render(renderer: Renderer) {
		renderer.draw(0,0,"Dealer's Hand:\n");

		if(this.playState === PlayState.PLAYER_TURN) renderer.draw(0,1,this.dealer.getConcealedHandAscii());
		else renderer.draw(0,1,this.dealer.getHandAscii());

		renderer.draw(0,9,`${this.player.name}'s Hand:\n`);
		renderer.draw(0,10,this.player.getHandAscii());
	}

	private dealCards(player: Player, count: number): void {
		for(let i = 0; i < count; i++) {
			player.dealCard(this.deck.getCard());
		}
	}

}
class Game {
	private renderer: Renderer;
	private stateStack: IState[];

	public constructor() {
		this.stateStack = [];
		this.renderer = new Renderer(50,18);
		this.enterState(new Playing());
	}

	public start(): void {
		while(this.stateStack[0]){
			this.renderer.clearBuffer();
			this.renderer.clearScreen();
			this.stateStack[0].render(this.renderer);
			this.renderer.show();
			this.stateStack[0].update();
		}
	}

	public enterState(state: IState): void {
		this.stateStack.push(state);
	}

	public exitState(): void {
		this.stateStack.shift();
	}

}
const game: Game = new Game();
game.start();
