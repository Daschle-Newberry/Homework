import readline from "readline-sync";
import * as Util from "./util";

class Card {
	public suit: string;
	public rank: string;
	public value: number;

	public constructor(suit: string, rank: string, value: number) {
		this.suit = suit;
		this.rank = rank;``
		this.value = value;
	}

	public frontFace(): string {
		// Ascii art heavily inspired by https://www.asciiart.eu/art/2eb26a8e809d26a8
		const numSpaces = 4 - (this.rank.length - 1);
		return 	` _____ \n`+
				`|${this.rank}${" ".repeat(numSpaces)}|\n`+
				`|     |\n`+
				`|  ${this.suit}  |\n`+
				`|     |\n`+
				`|${" ".repeat(numSpaces)}${this.rank}|\n`+
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
 		this.cards = [];
 		this.init();
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

 	public getCard(): Card {
 		const card: Card | undefined = this.cards.shift();

 		if(!card) throw new Error("Illegal Game State: Deck empty");
 		
 		return card;
 	}

 	public getCardCount(): number{
 		return this.cards.length;
 	}

 	public reset(): void {
		this.cards = [];
 		this.init();
 	}


 	private init(): void {
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
 }

class Player {
	protected name: string;
	protected bet: number;
	protected balance: number;
	protected hand: Card[];

	public constructor(name: string, bet: number, balance: number) {
		this.name = name;
		this.bet = bet;
		this.balance = balance;
		this.hand = [];
	}

	public hitStay(): boolean{
		if(this.getPoints() >= 21) return false;
		return "h" === readline.question("(h)it or (s)tay? ");
	}

	public dealCard(card: Card): void { 
		this.hand.push(card);
	}

	public resetHand(): void{
		this.hand = [];
	}

	public getHandAscii(): string {
		if(this.hand.length < 1) return "";

		const handString: string[] = [];
		for(const card of this.hand) {
			handString.push(card.frontFace());
		}
		
		return Util.concatByLine(handString) + `Points: ${this.getPoints()}\n`;
	}

	public getPoints(): number {
		let points: number = 0;
		let aces = 0;
		for(const c of this.hand) {
			points += c.value;
			if(c.rank === "A") aces++;
		}

		return this.aceCorrect(points, aces);

	}

	public changeBalance(delta: number): void {
		this.balance += delta;
	}

	public setBet(bet: number): void {
		this.bet = bet;
	}

	public getName(): string {
		return this.name;
	}

	public getBet(): number {
		return this.bet;
	}

	public getBalance(): number {
		return this.balance;
	}

	public getHandSize(): number {
		return this.hand.length;
	}

	//Technically this method could be static
	protected aceCorrect(points: number, aces: number): number {
		// Compute required aces based on how many changes of 10 are needed to be <= 21. 
		// points/10 - 2.1 == how many aces we need to swap to be 1.
		// Take the ceiling (can't have a part of an ace)
		// Take the max with 0 to avoid negative ace count when the hand is <= 21.
		const requiredAces =  Math.max(Math.ceil((points/10) - 2.1), 0);
		return points - Math.min(requiredAces,aces) * 10;
	}
}

class Dealer extends Player {
	private isHidden: boolean;
	constructor() {
		super("Dealer",0, 1000000);
		this.isHidden = true;
	}

	// Override, dealer's basic hit or stay algorithm
	public hitStay(): boolean {
		return this.getPoints() < 17;
	}

	public unhide(): void {
		this.isHidden = false;
	}

	public hide(): void {
		this.isHidden = true;
	}

	// Override, manages hiding cards when dealer is hidden
	public getHandAscii(): string {
		if(this.hand.length < 1) return "";
		
		const handString: string[] = [];

		let skip: number = 0;
		if(this.isHidden){
			skip = 1;
			handString.push(this.hand[0].backFace());
		}

		for(const card of this.hand.slice(skip)) {
			handString.push(card.frontFace());
		}
		
		let pointsString: string = `Points: ${this.getPoints()}`;
		if(this.isHidden) pointsString += " + ?";
		return Util.concatByLine(handString) + pointsString + "\n";
	}

	// Override, manages hiding points when dealer is hidden
	public getPoints(): number {
		//I know this violates DRY, logic could be factored out.

		let points: number = 0;
		let aces: number = 0;
		
		let skip: number = 0;
		if(this.isHidden) skip = 1; 
		
		for(const c of this.hand.slice(skip)) {
			if(c.rank === "A") aces++;
			points += c.value;
		}
		return this.aceCorrect(points, aces);

	}
}

interface IState {
	update(): void;
	render(renderer: Renderer): void;
	enter(): void;
}

class CharacterCreator implements IState  {
	private static banner = `
   _____                _                                 
  / ____|              | |                                
 | |     _ __ ___  __ _| |_ ___   _   _  ___  _   _ _ __  
 | |    | '__/ _ \\/ _\` | __/ _ \\ | | | |/ _ \\| | | | '__| 
 | |____| | |  __/ (_| | ||  __/ | |_| | (_) | |_| | |    
  \\_____|_|  \\___|\\__,_|\\__\\___|  \\__, |\\___/ \\__,_|_|    
                                   __/ |                  
   _____ _                        |___/                   
  / ____| |                        | |                    
 | |    | |__   __ _ _ __ __ _  ___| |_ ___ _ __          
 | |    | '_ \\ / _\` | '__/ _\` |/ __| __/ _ \\ '__|         
 | |____| | | | (_| | | | (_| | (__| ||  __/ |            
  \\_____|_| |_|\\__,_|_|  \\__,_|\\___|\\__\\___|_|            
                                                          
                                                                                                            
	`;

	public game: Game;

	constructor(game: Game){
		this.game = game;
	}

	public update(): void {
		const name: string = readline.question("Enter Your Gambler's Name: ");
		let balance: number = readline.questionInt("Enter Balance (-1 for default): ");
		let bet: number = readline.questionInt("Enter Bet (-1 for default): ");

		//Handle default or negative for balance or bet
		balance = Math.max(balance, 100);
		bet = Math.max(bet, .25 * balance);

		//Hate this
		this.game.setPlayer(new Player(name, bet, balance));

		this.game.replaceState(new MainMenu(this.game));
	}

	public render(renderer: Renderer): void {
		renderer.draw(0,0,CharacterCreator.banner);
	}

	public enter(): void {}

}

enum MenuState {
	MAIN,
	P_BROKE,
	C_BROKE,
}

class MainMenu implements IState{
	private banner: string =`





 ____  _            _           _            _    
|  _ \\| |          | |         | |          | |   
| |_) | | __ _  ___| | __      | | __ _  ___| | __
|  _ <| |/ _\` |/ __| |/ /  _   | |/ _\` |/ __| |/ /
| |_) | | (_| | (__|   <  | |__| | (_| | (__|   < 
|____/|_|\\__,_|\\___|_|\\_\\  \\____/ \\__,_|\\___|_|\\_\\
						by Daschle Newberry
`;


	private game: Game;
	private state: MenuState;

	constructor(game: Game) {
		this.state = MenuState.MAIN;
		this.game = game;
	}

	public update(): void {
		//Would have liked to do substates to handle this logic, time did not permit.
		switch(this.state){
			case MenuState.MAIN:
				const choice: string = readline.question("(p)lay (c)hange bet (q)uit ");
				if(choice === "p") {
					this.game.enterState (new Playing(this.game));
				}else if(choice === "c"){
					let newBet: number = readline.questionInt("Enter New Bet: ");
					newBet = Math.min(this.game.getPlayer().getBalance(), this.game.getCasino().getBalance(), newBet);
					this.game.getPlayer().setBet(newBet);
				}else{
					this.game.exitState();
				}
				break;
			default:
				this.game.exitState();
				break;
		}
	}

	public render(renderer: Renderer): void {
		//Would have liked to do substates to handle this logic, time did not permit.
		switch(this.state){
			case MenuState.MAIN:
				const stats: string = `
${this.game.getPlayer().getName()}'s Balance: ${this.game.getPlayer().getBalance()}		Casino's Balance: ${this.game.getCasino().getBalance()}

${this.game.getPlayer().getName()}'s Current Bet: ${this.game.getPlayer().getBet()}		Deck Size: ${this.game.getDeck().getCardCount()}

	`;
				renderer.draw(0,0,this.banner);
				renderer.draw(0,15,stats);
				break;
			case MenuState.P_BROKE:
				renderer.draw(0,0, "Player out of money!")
				break;
			case MenuState.C_BROKE:
				renderer.draw(0,0, "Casino out of money!");
				break;
		}
	}

	public enter(): void {
		//Change state of main menu based player/casino balance.
		if(this.game.getPlayer().getBalance() <= 0) this.state = MenuState.P_BROKE;
		else if(this.game.getCasino().getBalance() <= 0) this.state = MenuState.C_BROKE;
		else this.state = MenuState.MAIN; 
	}

}

class GameReport implements IState {
	private game: Game;
	private playerHand: string;
	private dealerHand: string;
	private report: string;

	constructor(game: Game, condition: WinLossCondition, playerHand: string, dealerHand: string) {
		this.game = game;
		this.playerHand = playerHand;
		this.dealerHand = dealerHand;
		this.report = condition;
	}

	public update(): void {
		readline.question("Press any key to continue...");
		this.game.exitState();
	}

	public render(renderer: Renderer): void {
		renderer.draw(0,0,"Dealer's Hand:\n");
		renderer.draw(0,1,this.dealerHand);

		renderer.draw(0,9,`player's Hand:\n`);
		renderer.draw(0,10,this.playerHand);
		renderer.draw(0,19,this.report);
	}

	public enter(): void {}
}

enum PlayState {
	PLAYER_TURN,
	DEALER_TURN,
	ROUND_OVER,
}

enum WinLossCondition {
	P_BUST = "Player Busts, Dealer Wins!",
	D_BUST = "Dealer Busts, Player Wins!",
	P_POINTS = "Player wins!",
	D_POINTS = "Dealer Wins!",
	PUSH = "Push!",
}

class Playing implements IState {
	private game: Game;

	private deck: Deck;
	private player: Player;
	private dealer: Dealer;
	private playState: PlayState;

	constructor(game: Game) {
		this.game = game;

		this.player = this.game.getPlayer();
		this.dealer = this.game.getCasino();
		this.deck = this.game.getDeck();

		this.playState = PlayState.PLAYER_TURN;
	}

	public update(): void {
		//I would have liked to do substates to handle this logic, but time did not permit.
		switch (this.playState) {
			case PlayState.PLAYER_TURN:
				if(this.player.hitStay()) {
					this.dealCards(this.player,1);
				} 
				else {
					this.dealer.unhide();

					if(this.player.getPoints() > 21) {
						this.playState = PlayState.ROUND_OVER;
					} 
					else {
						this.playState = PlayState.DEALER_TURN;
					}
				}
				break;
			case PlayState.DEALER_TURN:
				if(this.dealer.hitStay()) {
					this.dealCards(this.dealer,1);
				} 
				else {
					this.playState = PlayState.ROUND_OVER;
				}
				break;
			case PlayState.ROUND_OVER:
				const condition: WinLossCondition = this.handleWinner();
				this.game.replaceState(new GameReport(this.game, condition, this.player.getHandAscii(), this.dealer.getHandAscii()));
				break;
		}
	}

	public render(renderer: Renderer): void {
		renderer.draw(0,0,"Dealer's Hand:\n");
		renderer.draw(0,1,this.dealer.getHandAscii());

		renderer.draw(0,9,`${this.player.getName()}'s Hand:\n`);
		renderer.draw(0,10,this.player.getHandAscii());
	}

	public enter(): void {
		//Logic to handle resetting player and dealer upon entering the game
		if(this.deck.getCardCount() < 30) this.deck.reset();

		this.deck.shuffle();

		this.player.resetHand();
		this.dealer.resetHand();
		this.dealer.hide();

		this.dealCards(this.player, 2);
		this.dealCards(this.dealer, 2);
	}

	private handleWinner(): WinLossCondition {
		// This function makes my eyes bleed
		let payout: number = 0;
		let condition: WinLossCondition = WinLossCondition.PUSH;
		const playerPoints: number = this.player.getPoints();
		const dealerPoints: number = this.dealer.getPoints();
		if(playerPoints > 21){
			payout = -1;
			condition = WinLossCondition.P_BUST;
		}
		else if(dealerPoints > 21){
			payout = 1;
			condition = WinLossCondition.D_BUST;
		} 
		else if(dealerPoints > playerPoints){
			payout = -1;
			condition = WinLossCondition.D_POINTS;
		} 
		else if(dealerPoints < playerPoints){
			if(playerPoints === 21 && this.player.getHandSize() === 2){
				payout = 1.5;
			}else{
				payout = 1;
			}
			condition = WinLossCondition.P_POINTS;
		} 
		
		this.player.changeBalance(payout * this.player.getBet());
		this.dealer.changeBalance(-payout * this.player.getBet());

		return condition;
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

	// I really dislike these being here, but oh well.
	private player!: Player;
	private casino: Dealer;
	private deck: Deck;

	constructor(){
		this.stateStack = [];
		this.renderer = new Renderer(60,20);

		this.casino = new Dealer();
		this.deck = new Deck();

		this.enterState(new CharacterCreator(this));
	}

	public start(): void {
		let current: IState | undefined;
		while((current = this.stateStack.at(-1)) != undefined) {
			//Ordering here ensures the first game frame is rendered.
			this.renderer.clearBuffer();
			this.renderer.clearScreen();

			current.render(this.renderer);
			this.renderer.show();

			current.update();
		}
	}

	public enterState(state: IState): void {
		this.stateStack.push(state);
		state.enter();
	}

	public replaceState(state: IState): void {
		this.stateStack.pop();
		this.stateStack.push(state);
		state.enter();
	}

	public exitState(): void {
		this.stateStack.pop();
		this.stateStack.at(-1)?.enter();
	}

	public setPlayer(player: Player): void {
		this.player = player;
	}

	public getPlayer(): Player {
		return this.player;
	}

	public getCasino(): Dealer {
		return this.casino;
	}

	public getDeck(): Deck {
		return this.deck;
	}
}

// Basic renderer class to manage printing to console without creating new lines.
class Renderer {
	private width: number;
	private height: number;
	private buffer: string[][];

	constructor(width: number, height: number){
		this.width = width;
		this.height = height;
		this.buffer = [];
		for(let y = 0; y < this.height;  y++){
			this.buffer.push([]);
			for(let x = 0; x < this.width; x++){
				this.buffer[y].push("");
			}
		}
	}

	public draw(x: number, y: number, str: string): void {
		const chars: string[][] = str.split("\n").map((s) => s.split(""));
		
		for(let row = 0; row < chars.length && y + row < this.height; row++){
			for(let col = 0; col < chars[row].length && x + col < this.width; col++){
				this.buffer[y + row][x + col] = chars[row][col];
			}
		}
	}

	public show(): void { 
		const str: string = this.buffer.map((s) => s.join("")).join("\n");
		console.log(str);
	}
	public clearBuffer(): void {
		this.buffer = [];
		for(let y = 0; y < this.height;  y++){
			this.buffer.push([]);
			for(let x = 0; x < this.width; x++){
				this.buffer[y].push("");
			}
		}
	}

	public clearScreen(): void {
		console.clear();
	}
}

new Game().start();
