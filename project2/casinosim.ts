

interface IState {
	enter(): void;
	exit(): void;
	update(): void;
	render(renderer: Renderer): void;
}

interface IStateTransition {
	execute()
}

//Abstract class for handling Game logic, is also a state
abstract class Game implements IState {
	private name: string;
	private book: Map<Gambler, number>
	private gamePhase: IGamePhase;
	private app: Application;

	constructor(name: string, casino: Casino) {
		this.name = name;
		this.book = new Map();
		this.casion = casino;
		this.gameState = [];
	}


	public getName(): string {
		return this.name;
	}
}

class Casino implements IState {
	private app: Application;
	private 
	
	constructor(app: Application) {
		this.app = app;
	}

 	public enter(): void {
 	}

	public exit(): void {
	}

	public update(): void {
	}

	public render(renderer: Renderer): void {
	}
}


class Application {
	private stateStack: IState[];
	private renderer: Renderer;

	constructor() {
		this.stateStack = [];
		this. renderer = new Renderer(60,60);
		this.enterState(new Casino(this));
	}

	public start(): void {
		while(this.stateStack.length > 0) {
			const current = this.stateStack[this.stateStack.length - 1];
			current.render(this.renderer);
			current.update();
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
		
		for(let row = 0; row < chars.length && y + row < this.height; row++){
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