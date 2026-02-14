// *--------UTILITY--------*
function concatByLine(strs: string[]): string {
	const strsLines: string[][] = strs.map((str) => str.split("\n"));
	let res: string = "";

		for(let j = 0; j < strsLines[0].length; j++) {
			for (const lines of strsLines) {
				res += lines[j];
			}
			res += "\n";
		}
	return res;
}

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
				` ----- \n`;
	}

	public backFace(): string {
		return  ` _____ \n`+
				`|*~~~*|\n`+
				`|~   ~|\n`+
				`|~   ~|\n`+
				`|~   ~|\n`+
				`|*~~~*|\n`+
				` ----- \n`;
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

 	public getCard(): Card | undefined {
 		const card: Card | undefined = this.cards.shift();
 		return this.cards.shift();
 	}

 	public shuffle(): void { 
 		// Fisher-Yates shuffle
 		let index = this.cards.length - 1;
 		while(index > 0) {
 			const swapIndex: number = Math.floor(Math.random() * index);
 			const tmp: Card = this.cards[index];
 			this.cards[index] = this.cards[swapIndex];
 			this.cards[swapIndex] = tmp;
 			index--;
 		}
 	}
}

class Player {
	protected hand: Card[] = [];
	protected bet: number = 0;
	protected name: string;

	public constructor(name: string){
		this.name = name;
	}
	public setBet(bet: number): void {
		this.bet = bet;
	}

	public showHand(): void {
		if(this.hand.length < 1) return;

		console.log(`${this.name}'s Hand:`);

		const cardsStrings: string[] = [];
		for(const card of this.hand) {
			cardsStrings.push(card.frontFace());
		}
		
		console.log(concatByLine(cardsStrings));
	}

	public dealCard(card: Card) { 
		this.hand.push(card);
	}

	public getPoints() {
		let points = 0;
		let aceCount = 0;
		for(const c of this.hand) {
			if(c.type === "A") aceCount++;
			points += c.value;
		}

		const requiredAces =  Math.ceil((points/10) - 2.1);
		if(requiredAces > aceCount) return points;
		return points - requiredAces * 10;
	}
}

class Dealer extends Player {
	constructor() {
		super("Dealer");
	}

	public showHandConcealed(): void {
		if(this.hand.length < 1) return;
		

		console.log(`${this.name}'s Hand:`);

		const cardsStrings: string[] = [];
		cardsStrings.push(this.hand[0].backFace());

		for(const card of this.hand.slice(1)) {
			cardsStrings.push(card.frontFace());
		}
		
		console.log(concatByLine(cardsStrings));
	}



}

class Game {
	private deck: Deck;
	private player: Player;
	private dealer: Dealer;

	public constructor() {
		this.deck = new Deck();
		this.deck.shuffle();

		this.player = new Player("Jim");
		this.dealer = new Dealer();

		for(let i = 0; i < 3; i++) {
			const card: Card | undefined = this.deck.getCard();
			if(card === undefined) break;
			this.dealer.dealCard(card);
		}

		for(let i = 0; i < 3; i++) {
			const card: Card | undefined = this.deck.getCard();
			if(card === undefined) break;
			this.player.dealCard(card);
		}

	
		this.dealer.showHandConcealed();

		this.player.showHand();
	}

}       

let game: Game = new Game();

