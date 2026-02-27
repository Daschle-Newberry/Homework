"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const readline_sync_1 = __importDefault(require("readline-sync"));
class Gambler {
}
//Abstract class for handling Game logic, is also a state
class Game {
    constructor(name) {
        this.name = name;
        this.book = new Map();
        this.shouldExit = false;
    }
    enterState(state) {
        this.currentState = state;
    }
    //Same as enterState since game doesn't use a stack for states
    replaceState(state) {
        this.currentState = state;
    }
    //Flags game to exit.
    exitState() {
        this.shouldExit = true;
    }
}
class Betting {
    enter() { console.log("Entering Betting"); }
    exit() { console.log("Exiting Betting"); }
    update(manager) {
        console.log("Updating Betting");
        manager.exitState();
    }
    render(renderer) {
        renderer.clearBuffer();
    }
}
class TailsIWin extends Game {
    constructor() {
        super("TailsIWin");
        this.currentState = new Betting();
        this.currentState.enter();
    }
    enter() { console.log("Entering TailsIWin"); }
    exit() { console.log("Exiting TailsIWin"); }
    update(manager) {
        if (this.shouldExit) {
            manager.exitState();
            return;
        }
        this.currentState.update(this);
    }
    render(renderer) {
        renderer.clearBuffer();
    }
}
class Casino {
    constructor() {
    }
    enter() {
        console.log("Entering Casino");
    }
    exit() {
        console.log("Exiting Casino");
    }
    update(manager) {
        if (readline_sync_1.default.question("Start simulation? (y/n): ") == "y") {
            manager.enterState(new TailsIWin());
        }
        else {
            manager.exitState();
        }
    }
    render(renderer) {
        renderer.clearBuffer();
    }
}
class Application {
    constructor() {
        this.stateStack = [];
        this.renderer = new Renderer(60, 60);
        this.enterState(new Casino());
    }
    start() {
        while (this.stateStack.length > 0) {
            const current = this.stateStack[this.stateStack.length - 1];
            current.render(this.renderer);
            current.update(this);
        }
    }
    enterState(state) {
        this.stateStack.push(state);
        state.enter();
    }
    replaceState(state) {
        var _a;
        (_a = this.stateStack.pop()) === null || _a === void 0 ? void 0 : _a.exit();
        this.stateStack.push(state);
        state.enter();
    }
    exitState() {
        var _a;
        (_a = this.stateStack.pop()) === null || _a === void 0 ? void 0 : _a.exit();
        if (this.stateStack.length > 0) {
            this.stateStack[this.stateStack.length - 1].enter();
        }
    }
}
class Renderer {
    constructor(width, height) {
        this.width = width;
        this.height = height;
        this.buffer = [];
        for (let y = 0; y < this.height; y++) {
            this.buffer.push([]);
            for (let x = 0; x < this.width; x++) {
                this.buffer[y].push("");
            }
        }
    }
    draw(x, y, str) {
        const chars = str.split("\n").map((s) => s.split(""));
        for (let row = 0; row < chars.length && y + row < this.height; row++) {
            if (y + row > this.height)
                break;
            for (let col = 0; col < chars[row].length && x + col < this.width; col++) {
                if (x + col > this.width)
                    break;
                this.buffer[y + row][x + col] = chars[row][col];
            }
        }
    }
    show() {
        const str = this.buffer.map((s) => s.join("")).join("\n");
        console.log(str);
    }
    clearBuffer() {
        this.buffer = [];
        for (let y = 0; y < this.height; y++) {
            this.buffer.push([]);
            for (let x = 0; x < this.width; x++) {
                this.buffer[y].push("");
            }
        }
    }
    clearScreen() {
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
