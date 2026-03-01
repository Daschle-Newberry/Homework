import readline from "readline-sync";
interface IState {
	enter(): void;
	exit(): void;
	update(manager: StateManager): void;
	render(renderer: Renderer): void;
}

abstract class Gambler {
	private name: string;
	private money: number;

	private target: number;

	public constructor(name: string, money: number, target: number){
		this.name = name;
		this.money = money;
		this.target = target;
	}

	public hitTarget(): boolean {
		return this.money === this.target;
	}

	public isBankrupt(): boolean {
		return this.money >= 0;
	}

	public isFinished(): boolean {
		return this.hitTarget() || this.isBankrupt();
	}

	public abstract getBet(): number;
	public getName(): string {return this.name;}
	public getMoney(): number {return this.money;}
	public getTarget(): number {return this.target;}
	
}

class StableGambler extends Gambler {
	private bet: number;

	public constructor(name: string, money: number, bet: number) {
		super(name,money,bet * 2);
		this.bet = bet;
	}

	public getBet(): number {
		return Math.max(this.bet, this.getMoney());
	}
}

class StateManager {
	private stateStack: IState[];

	protected constructor() {
		this.stateStack = [];
	}

	public enterState(state: IState): void {
		this.stateStack.push(state);
		state.enter();
	}

	public replaceState(state: IState): void {
		this.stateStack.pop()?.exit();
		this.stateStack.push(state);
		state.enter();
	}

	public exitState(): void {
		this.stateStack.pop()?.exit();
		if(this.stateStack.length > 0){
			this.stateStack[this.stateStack.length - 1].enter();
		}
	}

	public getCurrentState(): IState | undefined {
		if(this.stateStack.length === 0) return undefined;
		return this.stateStack[this.stateStack.length - 1];
	}
}

//Abstract class for handling Game logic, is also a state
abstract class Game extends StateManager implements IState{
	//Abstract so the typescript is upset that we don't initialize it in Game's constructor
	private name: string;
	private book: Map<Gambler, number>;
	private players: Gambler[];

	public constructor(name: string, players: Gambler[]) {
		super();
		this.name = name;
		this.players = players;
		this.book = new Map();
	}

	abstract enter(): void;
	abstract exit(): void;
	abstract update(manager: StateManager): void;
	abstract render(renderer: Renderer): void;

	public getName(): string {return this.name;}
	public getBook(): Map<Gambler, number> {return this.book;}	
}

class InfoScreen implements IState {
	private info : string;
	private title: string;

	public constructor(title: string, info: string) {
		this.info = info;
		this.title = title;
	}

	public enter(): void {console.log("Entering InfoScreen")}
	public exit(): void {console.log("Exiting InfoScreen")}

	public update(manager: StateManager): void {
		if(readline.question("Continue? (y/n): ") === "y"){
			manager.exitState();
		}
	}
	public render(renderer: Renderer): void {
		console.log(this.title);
		for(const [name, num] of this.info) {
			console.log(`${name}: ${num}`)
		}
	}
}

class GuessTheNumber extends Game {
	public constructor(players: Gambler[]) {
		super("GuessTheNumber", players);
		let info = ""
		for(const [gambler, bet] of this.getBook()) info += `${gambler}: $${bet}\n`
		this.enterState(new InfoScreen("Bets: \n",info));
	}

	public enter(): void {console.log("Entering GuessTheNumber")}
	public exit(): void {console.log("Exiting GuessTheNumber")}

	public update(manager: StateManager): void {
		const currentState: IState | undefined = this.getCurrentState();
		if(!currentState){
			manager.exitState();
			return;
		}
		currentState.update(this);
	}
	public render(renderer: Renderer): void {
		this.getCurrentState()?.render(renderer);
	}
}

class Casino implements IState {
	private gamblers: Gambler[];
	
	public constructor() {
		this.gamblers = [new StableGambler("Bob", 10, 5)]
	}

 	public enter(): void {
		console.log("Entering Casino");
 	}

	public exit(): void {
		console.log("Exiting Casino");
	}

	public update(manager: StateManager): void {
		if(readline.question("Start simulation? (y/n): ") == "y"){
			manager.enterState(new GuessTheNumber(this.gamblers));
		}else {
			manager.exitState();
		}
	}

	public render(renderer: Renderer): void {
		renderer.clearBuffer();
	}
}


class Application extends StateManager {
	private renderer: Renderer;

	public constructor() {
		super();
		this. renderer = new Renderer(50,10,50,1);
		this.enterState(new Casino());
	}

	public start(): void {
		let currentState: IState | undefined = this.getCurrentState();
		while(currentState) {
			this.renderer.clearBuffer();
			this.renderer.clearScreen();
			currentState.render(this.renderer);
			this.renderer.show();

			currentState.update(this);
			currentState = this.getCurrentState();
		}
		
	}

}

class Renderer {
	private width: number;
	private height: number;
	private debugWidth: number;
	private debugHeight: number;
	private buffer: string[][];

	public constructor(width: number, height: number, debugWidth: number = 0, debugHeight: number = 0){
		this.width = width;
		this.height = height;
		this.debugWidth = debugWidth;
		this.debugHeight = debugHeight;
		this.buffer = [];
		for(let y = 0; y < this.height + this.debugHeight;  y++){
			this.buffer.push([]);
			for(let x = 0; x < this.width + this.debugWidth; x++){
				this.buffer[y].push("");
			}
		}
	}

	public draw(x: number, y: number, str: string): void {
		const chars: string[][] = str.split("\n").map((s) => s.split(""));
		
		for(let row = 0; row < chars.length && y + row < this.height; row++) {
			if(y + row > this.height) break;
			for(let col = 0; col < chars[row].length && x + col < this.width; col++){
				if(x + col > this.width) break;
				this.buffer[y + row][x + col] = chars[row][col];
			}
		}
	}

	public drawDebug(x: number, y: number, str: string): void {
		const chars: string[][] = str.split("\n").map((s) => s.split(""));
		
		for(let row = 0; row < chars.length && y + row < this.debugHeight; row++) {
			if(y + row > this.height) break;
			for(let col = 0; col < chars[row].length && x + col < this.debugWidth; col++){
				if(x + col > this.width) break;
				this.buffer[y + row + this.height][x + col + this.width] = chars[row][col];
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

new Application().start();



// Application
//    |-> States
//			|-> Casino (Main Menu)
//			|-> Game
//				|-> Substate 
//						|-> PlaceBet
//						|-> Simulating
//						|-> Results		

// States must interact with their manager in some way.
// Idea: States can return an object, which details a command for the application
// Idea: Update can pass application as a parameter
// Idea: States can hold application as a field
// Idea: States can return a flag



// Chosen Solution:
// 		A state machine/state manager will inject itself as a parameter into the update
//					-> If a state wants to change the state of the FSM, they will request it via the passed manager
//					-> Game states which are also FSM/State mangers will extend the manager class
//

// If games are NOT state machines, they will keep their state on the application state stack
//
//[BOTTOM] Casino -> ResultsG1 -> PlayingG1 -> PlacingBetsG1 -> ResultsG2 -> PlayingG2 -> PlacingBetsG2 -> .... [TOP]

//Each game will lead into the next, each state will "own" the players, allowing the state to update the players.
//
//Can factor out ResutlsG_ and PlacingBetsG_ with InfoScreen
//[BOTTOM] Casino -> InfoScreen -> PlayingG1 -> InfoScreen -> InfoScreen -> PlayingG2 -> InfoScreen -> .... [TOP]

//But then Casino has to manage info screens for each game, which will make casino have game logic.

//If games ARE state machines, they  can keep their internal state on their own stack
//[BOTTOM] Casino -> G1 		-> 									G2 ->  .... [TOP]
// 					  |-> InfoScreen -> Playing -> InfoScreen		|-> InfoScreen -> Playing -> InfoScreen
//
// Since an infoscreen or playing state will not be returned, we can use a simple state machine without a stack
////[BOTTOM] Casino -> G1 		-> 									G2 ->  .... [TOP]
//						|-> currentState: InfoScreen | Playing		|-> currentState: InfoScreen | Playing
//
// Requires an extra class which owns its substates, but fixes the issue of the casino needing to manage game logic.
//
// Or we can have each Game handle its logic internally, with no extra states
//[BOTTOM] Casino -> G1 -> G2 ->  .... [TOP]
// This is WAY messier though, requires a lot of switch statements.
