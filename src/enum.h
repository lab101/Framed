#pragma once

enum ToolState{
	BRUSH, LINE, CIRCLE, RECTANGLE
};


inline std::string getToolStateString(ToolState state){

    std::string stateString = "line";

    switch (state) {
        case BRUSH:
            stateString = "brush";
            break;
        case LINE:
            stateString = "line";
            break;
        case CIRCLE:
            stateString = "circle";
            break;
        case RECTANGLE:
            stateString = "rectangle";
            break;

    }

    return stateString;
}


inline ToolState getToolStateByString(std::string name){

    ToolState state = LINE;

    if (name == "brush") state = ToolState::BRUSH;
    if (name == "line") state = ToolState::LINE;
    if (name == "circle") state = ToolState::CIRCLE;
    if (name == "rectangle") state = ToolState::RECTANGLE;

    return state;
}

