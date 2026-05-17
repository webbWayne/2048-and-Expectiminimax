from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
import subprocess
import time

# 1. Open the browser and go to the game
driver = webdriver.Chrome()
driver.get("https://2048.io/")
time.sleep(2) # Wait for page to load

body = driver.find_element(By.TAG_NAME, 'body')

def read_board():
    """Reads the HTML DOM to figure out where the tiles are."""
    matrix = [[0]*4 for _ in range(4)]
    
    # 2048.io stores tiles inside a container with class 'tile'
    tiles = driver.find_elements(By.CLASS_NAME, 'tile')
    
    for tile in tiles:
        classes = tile.get_attribute('class').split()
        val = 0
        col = 0
        row = 0
        
        # Parse the classes like 'tile-8' and 'tile-position-1-2'
        for c in classes:
            if c.startswith('tile-') and c[5:].isdigit():
                val = int(c[5:])
            if c.startswith('tile-position-'):
                parts = c.split('-')
                col = int(parts[2]) - 1 # DOM is 1-indexed (1-4)
                row = int(parts[3]) - 1
                
        matrix[row][col] = val
        
    return matrix

def get_best_move(matrix):
    """Sends the board to C++ and gets the move back."""
    # Flatten the matrix into a single string of 16 numbers
    input_str = " ".join(str(cell) for row in matrix for cell in row)
    
    # Run the C++ executable (Change './ai_brain' if on Windows to 'ai_brain.exe')
    process = subprocess.Popen(
        ['./2048_bot'], 
        stdin=subprocess.PIPE, 
        stdout=subprocess.PIPE, 
        text=True
    )
    
    # Send the board state and read the output
    output, _ = process.communicate(input_str)
    return output.strip()

# --- MAIN GAME LOOP ---
print("Bot starting...")
while True:
    try:
        # Read current state
        board = read_board()
        
        # Get move from C++
        move = get_best_move(board)
        print(f"AI chose: {move}")
        
        # Execute the move in the browser
        if move == 'l':
            body.send_keys(Keys.ARROW_LEFT)
        elif move == 'r':
            body.send_keys(Keys.ARROW_RIGHT)
        elif move == 'u':
            body.send_keys(Keys.ARROW_UP)
        elif move == 'd':
            body.send_keys(Keys.ARROW_DOWN)
            
        # CRITICAL: Wait for the CSS sliding animation to finish
        # If you don't wait, Python reads the board while tiles are moving and crashes
        time.sleep(0.15) 
        
    except Exception as e:
        print(f"Game over or error: {e}")
        break
