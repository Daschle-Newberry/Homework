import * as Util from "./utility";
import readline from "readline-sync";
import fs from "fs";

interface IGambler {
    isFinished(): boolean;
    getBet(): number;
    getName(): string;
    getMoney(): number;
    addMoney(amount: number): void;
    removeMoney(amount: number): void;
}

//Base gambler class which holds functions and fields common to all gambler types
class GamblerBase {
    private name: string;
	private money: number;
	private target: number;

    public constructor(name: string, money: number, target: number) {
        this.name = name;
        this.money = money;
        this.target = target;
	}

    public hitTarget(): boolean {
        return this.money == this.target;
    }

    public isBankrupt(): boolean {
        return this.money <= 0;
    }

    public isFinished(): boolean {
        return this.hitTarget() || this.isBankrupt();
    };

    getName(): string { return this.name; }
    getMoney(): number { return this.money; }
    addMoney(amount: number): void {this.money += amount; }
    removeMoney(amount: number): void {this.money -= amount; }

}

class StableGambler implements IGambler {
    private gamblerBase: GamblerBase;
	private bet: number;

	public constructor(name: string, money: number, bet: number) {
        this.gamblerBase = new GamblerBase(name,money,money * 2);   
        this.bet = bet;
	}

    public isFinished(): boolean {
        return this.gamblerBase.isFinished();
    }
    public getBet(): number {
		return Math.min(this.bet, this.gamblerBase.getMoney());
	}
    
    public getName(): string { return this.gamblerBase.getName(); }
    public getMoney(): number { return this.gamblerBase.getMoney(); }
    public addMoney(amount: number): void { this.gamblerBase.addMoney(amount); }
    public removeMoney(amount: number): void { this.gamblerBase.removeMoney(amount); }
}

class HighRiskGambler implements IGambler {
    private gamblerBase: GamblerBase;
	private yoloAmount: number;

	public constructor(name: string, money: number, yoloAmount: number) {
        this.gamblerBase = new GamblerBase(name,money,money * 5);   
        this.yoloAmount = yoloAmount;
	}

    public isFinished(): boolean {
        return this.gamblerBase.isFinished();
    }
    public getBet(): number {
		const desiredBet: number = this.getMoney() < this.yoloAmount ? this.getMoney() : .5 * this.getMoney();
		return Math.min(this.getMoney(), desiredBet);
	}
    
    public getName(): string { return this.gamblerBase.getName(); }
    public getMoney(): number { return this.gamblerBase.getMoney(); }
    public addMoney(amount: number): void { this.gamblerBase.addMoney(amount); }
    public removeMoney(amount: number): void { this.gamblerBase.removeMoney(amount); }
}

class StreakGambler implements IGambler {
    private gamblerBase: GamblerBase;
	private bet: number;
	private minimumBet: number;
	private winMultiplier: number;
	private lossMultiplier: number;

	public constructor(
			name: string,
			money: number,
			initialBet: number, 
			minimumBet: number,
			winMultiplier: number,
			lossMultiplier: number,
			target: number,
		){
			this.gamblerBase = new GamblerBase(name,money,target);
			this.bet = initialBet;
			this.minimumBet = minimumBet;
			this.winMultiplier = winMultiplier;
			this.lossMultiplier = lossMultiplier;
		}

        public isFinished(): boolean {
            return this.gamblerBase.isFinished();
        }

		public getBet(): number {
			const desiredBet: number = this.bet < this.minimumBet ? this.minimumBet : this.bet;
			return Math.min(this.gamblerBase.getMoney(), desiredBet);
		}

		public addMoney(amount: number): void {
			this.gamblerBase.addMoney(amount);
			this.bet *= this.winMultiplier;
		}
		public removeMoney(amount: number): void {
			this.gamblerBase.removeMoney(amount);
			this.bet *= this.lossMultiplier;
		}

        public getName(): string { return this.gamblerBase.getName(); }
        public getMoney(): number { return this.gamblerBase.getMoney(); }
}


class GamblerFactory {
	private config: any;

	public constructor(config: string) {
		this.config = JSON.parse(fs.readFileSync(config,"utf-8"));

	}
	public createGamblers(count: number): IGambler[] {
		let gamblers: IGambler[] = []
		for(let i = 0; i < count; i++) {
			const randType: number = Util.randInRange(1,4, true);
			const randName: string = this.config.names[Math.ceil(Math.random() * this.config.names.length)]
			switch(randType) {
				case 1: {
					//Stable
					const balance: number = Util.randInRange(this.config.stable.balance.lo, this.config.stable.balance.hi);
					const betPercent: number = Util.randInRange(this.config.stable.bet.lo, this.config.stable.bet.hi);
					gamblers.push(new StableGambler(randName, balance, balance * betPercent));
					break;
				}
				case 2: {
					//High Risk
					const balance: number = Util.randInRange(this.config.highrisk.balance.lo, this.config.highrisk.balance.hi);
					const yoloPercent: number = Util.randInRange(this.config.highrisk.yolo.lo,this.config.highrisk.yolo.hi);
					gamblers.push(new HighRiskGambler(randName, balance,balance * yoloPercent));
					break;
				}
				case 3: {
					//Streak
					const balance: number = Util.randInRange(this.config.streak.balance.lo, this.config.streak.balance.hi);
					const betPercent: number = Util.randInRange(this.config.streak.bet.lo, this.config.streak.bet.hi);
					const minBetPercent: number = Util.randInRange(this.config.streak.minbet.lo, this.config.streak.minbet.hi);
					const winMultiplier: number = Util.randInRange(this.config.streak.winmult.lo, this.config.streak.winmult.hi);
					const lossMultiplier: number = Util.randInRange(this.config.streak.lossmult.lo, this.config.streak.lossmult.hi);
					gamblers.push(new StreakGambler(
									randName,
									balance,
									balance * betPercent, 
									balance * betPercent * minBetPercent,
									winMultiplier,
									lossMultiplier,
									balance * 6,
								)
							);
					break;
				}
			}
		}

		return gamblers;
	}

}

//Helper function to print a map with a string in between the key and values
function printChoice(book: Map<IGambler, number>, text: string): void {
    book.forEach(
        (choice: number, g: IGambler) => console.log(`${g.getName()}${text}${choice.toFixed(2)}`)
    );
}


//Takes gambler's bets and choices, and increments their money by the payout percent based on if they win or not.
function printAndUpdateWinners(book: Map<IGambler, number>, choices: Map<IGambler, number>, outcome: number, payout: number): number {
    let casinoMoney = 0;
    choices.forEach((choice: number, g: IGambler) => {
            const bet: number | undefined = book.get(g);
            if(bet == undefined) throw new Error(`Gambler present in game but not in book (Illegal State)`);

            if(choice == outcome) {
                let winnings = payout * bet;
                g.addMoney(winnings);
                casinoMoney -= winnings;
                console.log(`${g.getName()} won $${winnings.toFixed(2)}`);
            }
            else {
                g.removeMoney(bet);
                casinoMoney += bet;
            }
        })
    return casinoMoney;
} 

interface ISimulation {
    simulate(book: Map<IGambler, number>): number;
}

class TailsIWin implements ISimulation {
    private name: string;
    private outcome: number;

    public constructor() {
        this.name = "Tails I Win";
        this.outcome = Util.randInRange(0,1, true);
    }

    public simulate(book: Map<IGambler, number>): number {
        console.log(this.name);

        printChoice(book, " is betting $");

        //Map of heads vs tail choices (they can only pick heads)
        const choices: Map<IGambler, number> = new Map(
            Array.from(book.keys()).map((g: IGambler) => [g, Util.randInRange(0,0,true)])
        );

        console.log(`\nThe coin lands on ${this.outcome == 0 ? "HEADS" : "TAILS"}\n`);

        //Print and update winners
        const casinoMoney = printAndUpdateWinners(book, choices, this.outcome, 1.9);

        return casinoMoney;
    } 
}


class GuessTheNumber implements ISimulation {
    private name: string;
    private outcome: number;

    public constructor() {
        this.name = "Guess The Number";
        this.outcome = Util.randInRange(0,4, true);
    }

    public simulate(book: Map<IGambler, number>): number {
        console.log(this.name);

        printChoice(book," is betting $");
        
        //Create map of number choices
        const choices: Map<IGambler, number> = new Map(
            Array.from(book.keys()).map((g: IGambler) => [g, Util.randInRange(0,4,true)])
        );

        printChoice(choices," guesses ");

        console.log(`\nThe number is ${this.outcome}\n`);

        const casinoMoney = printAndUpdateWinners(book, choices, this.outcome, 3.5);

        return casinoMoney;
    } 
}

class OffTrackGPR implements ISimulation {
    private name: string;
    private outcome: number;
    private payout: number;
    public constructor() {
        this.name = "Offtrack Guinea Pig Racing";
        this.outcome = Util.weightedRandomIndex([.50,.25,.125,.125])
        
        const payouts = [1.9,3.8,7.6,7.6];
        this.payout = payouts[this.outcome];
    }

    public simulate(book: Map<IGambler, number>): number {
        console.log(this.name);

        printChoice(book," is betting $");
        
        //Create map of pig choices 
        const choices = new Map(
            Array.from(book.keys()).map((g: IGambler) => [g, Util.randInRange(0,4,true)])
        );

        printChoice(choices," is betting on Pig #");

        console.log(`\nPig #${this.outcome} won\n`);

        //Print and update winners
        const casinoMoney = printAndUpdateWinners(book, choices, this.outcome, this.payout);

        return casinoMoney;
    } 
}


class CasinoSimulator {
    private static MAX_ROUNDS = 1000;
    private games: ISimulation[];
    private gamblers: IGambler[];

    public constructor() {
        //Create game flow and gamblers
        this.games = [new TailsIWin(), new OffTrackGPR(), new GuessTheNumber()];
        this.gamblers = new GamblerFactory("gamblers.json").createGamblers(10);
    }

    public start(): void {
        let current_round = 1;

        while(current_round <= CasinoSimulator.MAX_ROUNDS) {
            console.log(`Round ${current_round}`);
            
            //Print gamblers and their remaining balance
            this.gamblers.forEach((g: IGambler) => console.log(`${g.getName()} has $${g.getMoney().toFixed(2)} remaining`));

            readline.question("Press any key to start round...\n")

            for(const game of this.games) {
                const book: Map<IGambler, number> = new Map(this.gamblers.map((g: IGambler) => [g, g.getBet()]));

                const casinoMoney: number = game.simulate(book);
                console.log(`Casino made/lost $${casinoMoney.toFixed()}`);

                //Cull gamblers and print who left
                const leavers: IGambler[] = this.cull();
                leavers.forEach((g: IGambler) => console.log(`${g.getName()} left`));
                readline.question("Press any key to next game...\n")

            }
            current_round++;
        }
    }

    public cull(): IGambler[] {
        //Mask out only gamblers who left
        const leavers: IGambler[] = this.gamblers.filter((g: IGambler) => g.isFinished());

        //Remaining gamblers are those who are not finished
        this.gamblers = this.gamblers.filter((g: IGambler) => !g.isFinished());
        return leavers;
    }
}

new CasinoSimulator().start();



