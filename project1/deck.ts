export class Card {
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

export class Deck {
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
