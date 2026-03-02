import readline from "readline"
import fs from "fs"

interface IState {
	enter(): void;
	exit(): void;
	update(manager: StateManager, keyHandler : KeyEventHandler): void;
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
	abstract update(manager: StateManager, keyHandler : KeyEventHandler): void;
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

	public update(manager: StateManager, keyHandler : KeyEventHandler): void {
		const currentState: IState | undefined = this.getCurrentState();
		if(!currentState){
			manager.exitState();
			return;
		}
		currentState.update(this, keyHandler);
	}
	public render(renderer: Renderer): void {
		this.getCurrentState()?.render(renderer);
	}
}

class Casino implements IState {
	private tui: Tui;
	private gamblers: Gambler[];
	private y: number = 0;
	
	public constructor() {
		this.gamblers = [new StableGambler("Bob", 10, 5)]
		this.tui = new Tui("tui.json","casino");
	}

 	public enter(): void {
		console.log("Entering Casino");
 	}

	public exit(): void {
		console.log("Exiting Casino");
	}

	public update(manager: StateManager, keyHandler : KeyEventHandler): void {
	}

	public render(renderer: Renderer): void {
		this.tui.getComponents().forEach(component => {
			renderer.draw(component.getX(),component.getY(),component.toString());
		});
	}
}
interface TuiSpecification {
	type: string;
	x: number | string;
	y: number | string;
	formatting: string;
	visual: string[];
}

class TuiComponent {
	private x: number;
	private y: number;
	private formatting: string;
	private visual: string[];

	//         1
	//00000000000000000000
	constructor(spec: TuiSpecification) {
		this.formatting = spec.formatting;
		this.visual = spec.visual;

		let maxLineLength: number = Math.max(...this.visual.map((str) => str.length));
		this.visual = this.visual.map((str) => str + " ".repeat(maxLineLength - str.length));

		if(typeof spec.x === "string") {
			if(spec.x === "center") {
				this.x = Math.ceil((Application.width - maxLineLength) / 2);
				console.log(this.x);
			} else {
				throw new Error(`Unknown alignment type "${spec.x}"`);
			}
		}else {
			this.x = spec.x;
		}

		if(typeof spec.y === "string") {
			if(spec.y === "center") {
				this.y = Math.ceil((Application.height - this.visual.length) / 2);
			} else {
				throw new Error(`Unknown alignment type "${spec.y}"`);
			}
		}else {
			this.y = spec.y;
		}

	}

	public toString(): string {
		return(
			this.visual
			.map(line => `${this.formatting}${line}\u001b[0m`)
			.join("\n")
			)
	}

	public getX(): number {return this.x}
	public getY(): number {return this.y}


}

class Tui {
	private components: TuiComponent[];

	constructor(file: string, state: string) {
		const tuiJSON: any = JSON.parse(fs.readFileSync("tui.json","utf-8"));
		this.components = tuiJSON[state].map((spec: TuiSpecification) => new TuiComponent(spec));
	}

	public addComponenet(component: TuiComponent): void {
		this.components.push(component);
	}
	public getComponents(): TuiComponent[] {
		return this.components;
	}

}
class Application extends StateManager {
	private static FPS = 5;
	private static MSPF = 1000 / this.FPS;
	public static height = 10;
	public static width = 10;

	private renderer: Renderer;
	private keyHandler: KeyEventHandler;

	public constructor() {
		super();
		this.renderer = new Renderer(Application.width,Application.height);
		this.keyHandler = new KeyEventHandler();

		if(!process.stdin.isTTY) {
			throw new Error("Cannot run in a non-TTY console");
		}
		
		readline.emitKeypressEvents(process.stdin)
		process.stdin.setRawMode(true);
		process.stdin.resume();
		process.stdin.on("keypress", (str, key) => {
			this.keyHandler.handle(str,key);
		})
		

		this.enterState(new Casino());
	}

	public start(): void {
		let currentState: IState | undefined = this.getCurrentState();
		const loop = () => {
			if(currentState == undefined) return;
	
			this.renderer.clear();
			currentState.render(this.renderer);
			// this.renderer.draw(0,0,"Hello!");
			this.renderer.show();
			// currentState.update(this, this.keyHandler);

			// currentState = this.getCurrentState();
			
			// Loop again after events are handled.
			setTimeout(loop, Application.MSPF);
		}
		
		loop();
	}



}

type Key = readline.Key;

class KeyEventHandler {
	private wasPressed: Set<string>;

	constructor() {
		this.wasPressed = new Set();
	}

	public handle(str: string, key: Key): void {
		if(key.ctrl && key.name === "c") process.exit();
		this.wasPressed.add(str);
	}

	public keyPressed(key: string){
		return this.wasPressed.has(key);
	}

}


class Buffer {
	private width: number;
	private height: number;
	private pixels: string[][];

	public constructor(width: number, height: number){
		this.width = width;
		this.height = height;

		

		this.pixels = [];

		this.clear();
	}


	public toString(): string {
		return this.pixels.map((s) => s.join("")).join("\n");
	}
	
	public clear(): void {
		this.pixels = [];
		for(let y = 0; y < this.height;  y++){
			this.pixels.push([]);
			for(let x = 0; x < this.width; x++){
				this.pixels[y].push(" ");
			}
		}
	}

	public setPixel(x: number, 
					y: number, 
					char: string, 
					): void {
		if(x < 0 || x > this.width) return;
		if(y < 0 || y > this.height) return;
		this.pixels[y][x] = char;
	}

	public getDim(): [number, number] {return [this.width, this.height];}


}
class Renderer {
	// NEED TO FIX STORAGE FOR ESCAPE SEQUENCES IN STRINGS 
	private buffer: Buffer;

	public constructor(width: number, height: number){
		this.buffer = new Buffer(width, height);
	}

	private drawToRegion(x: number, 
						 y: number, 
						 str: string
						): void {``
		const chars: string[][] = str.split("\n").map((s) => s.split(""));
		//TO-DO: Optimize this for early stop if we overrun the buffer
		for(let row = 0; row < chars.length; row++) {
			for(let col = 0; col < chars[row].length; col++){
				this.buffer.setPixel(
					x + col, 
					y + row,
					chars[row][col]
				);
			}
		}
	}
	public draw(x: number, y: number, str: string): void {
		this.drawToRegion(x, y, str);
	}

	public drawDebug(x: number, y: number, str: string): void {
		//TO-DO: Add debug buffer or debug regions in main buffer
		throw new Error("Debug Drawing is not Implmented!")
	}

	public show(): void { 
		console.log(this.buffer.toString());
	}
	
	public clear(): void {
		console.clear();
		this.buffer.clear();
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
//[BOTTOM] Casino -> ResultsG1 -> PlayingG1 -> PlacingBetsGG1 -> ResultsG2 -> PlayingG2 -> PlacingBetsG2 -> .... [TOP]

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
