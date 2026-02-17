import { Deck, Card } from "./player";
import * as Util from "./util";

export class Player {
	public name: string;
	protected bet: number = 0;
	protected hand: Card[] = [];

	public constructor(name: string) {
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
		
		console.log(Util.concatByLine(cardsStrings) + `Points: ${this.getPoints()}\n`);
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

export class Dealer extends Player {
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
		
		console.log(Util.concatByLine(cardsStrings) + `Points: ${this.getPointsConcealed()} + ?\n`);
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
