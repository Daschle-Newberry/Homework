import readline from "readline-sync";

interface IStateManager{
	enterState(state: IState): void;
	replaceState(state: IState): void;
	exitState(): void;
}

interface IState {
	enter(): void;
	exit(): void;
	update(manager: IStateManager): void;
	render(renderer: Renderer): void;
}

abstract class Gambler {
	//TO-DO: implement me
}

//Abstract class for handling Game logic, is also a state
abstract class Game implements IStateManager, IState{
	protected abstract currentState: IState;

	protected name: string;
	protected book: Map<Gambler, number>;
	protected shouldExit: boolean;

	constructor(name: string) {
		this.name = name;
		this.book = new Map();
		this.shouldExit = false;
	}

	abstract enter(): void;
	abstract exit(): void;
	abstract update(manager: IStateManager): void;
	abstract render(renderer: Renderer): void;

	public enterState(state: IState): void{
		this.currentState = state;
	}

	//Same as enterState since game doesn't use a stack for states
	public replaceState(state: IState): void{
		this.currentState = state;
	}

	//Flags game to exit.
	public exitState(): void {
		this.shouldExit = true;
	}
}

class Betting implements IState {
	public enter(): void {console.log("Entering Betting")}
	public exit(): void {console.log("Exiting Betting")}
	public update(manager: IStateManager): void {
		console.log("Updating Betting")
		manager.exitState();
	}
	public render(renderer: Renderer): void {
		renderer.clearBuffer();
	}
}

class TailsIWin extends Game {
	protected currentState: IState;


	constructor() {
		super("TailsIWin");
		this.currentState = new Betting();
		this.currentState.enter();
	}

	public enter(): void {console.log("Entering TailsIWin")}
	public exit(): void {console.log("Exiting TailsIWin")}
	public update(manager: IStateManager): void {
		if(this.shouldExit){
			manager.exitState();
			return;
		}
		this.currentState.update(this);
	}
	public render(renderer: Renderer): void {
		renderer.clearBuffer();
	}
}

class Casino implements IState {
	
	constructor() {
	}

 	public enter(): void {
		console.log("Entering Casino");
 	}

	public exit(): void {
		console.log("Exiting Casino");
	}

	public update(manager: IStateManager): void {
		if(readline.question("Start simulation? (y/n): ") == "y"){
			manager.enterState(new TailsIWin());
		}else {
			manager.exitState();
		}
	}

	public render(renderer: Renderer): void {
		renderer.clearBuffer();
	}
}


class Application implements IStateManager {
	private stateStack: IState[];
	private renderer: Renderer;

	constructor() {
		this.stateStack = [];
		this. renderer = new Renderer(60,60);
		this.enterState(new Casino());
	}

	public start(): void {
		while(this.stateStack.length > 0) {
			const current = this.stateStack[this.stateStack.length - 1];
			current.render(this.renderer);
			current.update(this);
		}
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
}

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
		
		for(let row = 0; row < chars.length && y + row < this.height; row++) {
			if(y + row > this.height) break;
			for(let col = 0; col < chars[row].length && x + col < this.width; col++){
				if(x + col > this.width) break;
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