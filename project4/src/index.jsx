import React, { StrictMode, Component } from "react";
import { BrowserRouter, Routes, Route } from 'react-router-dom';
import { createRoot } from "react-dom/client";
import { Game1 } from "./games/game1";
import "./styles/index.css";
class App extends Component {
    render() {
        return <div>
                    <a href="/game1">Click here!</a>
            <p id="welcome-message">welcome to my web app.</p>
        </div>;
    }
}
const rootElem = document.getElementById('root');
if (rootElem == null) {
    throw new Error("No root element found in HTML file.");
}
const root = createRoot(rootElem);
root.render(<StrictMode>
        <BrowserRouter>
            <Routes>
                <Route path="/" element={<App />}/>
                <Route path="/game1" element={<Game1 />}/>
            </Routes>
        </BrowserRouter>
    </StrictMode>);
