# MCTS Directory Context Documentation

## Purpose Statement
The mcts directory contains the Monte Carlo Tree Search (MCTS) implementation for the YGO Agent system, providing AlphaZero-style agents and core MCTS algorithms as an alternative AI approach to reinforcement learning. This includes both Python and C++ implementations for performance-critical components.

## Core Abstractions

### `mcts/core/mcts.py` - Core MCTS Implementation

#### File Purpose
Contains the core Monte Carlo Tree Search algorithm implementation with UCT selection, tree expansion, and backpropagation.

#### Functions

**Name**: `mcts_search`
**Purpose**: Performs Monte Carlo Tree Search from a given game state
**Input Schema**:
- state: GameState - [Current game state]
- num_simulations: int - [Number of MCTS simulations to perform]
- exploration_constant: float - [UCT exploration constant]

**Output Schema**:
- Return Value: Dict[int, float] - [Action probabilities from MCTS]

**Side Effects**: 
- Memory: Allocates search tree nodes
- CPU: Intensive computation during simulations

**Error Conditions**: 
- ValueError: Invalid game state
- RuntimeError: Memory allocation failures
- AssertionError: Invalid search parameters

**Business Rules**: 
- Uses UCT (Upper Confidence Bound for Trees) selection
- Performs random rollouts for leaf evaluation
- Backpropagates results up the tree
- Returns visit count-based action probabilities

**Performance Notes**: 
- Time complexity: O(num_simulations * tree_depth)
- Memory usage scales with tree size
- CPU-intensive computation

#### Classes

**Name**: `MCTSNode`
**Responsibility**: Represents a node in the MCTS search tree
**State Schema**:
- state: GameState - [Game state at this node]
- parent: Optional[MCTSNode] - [Parent node]
- children: Dict[int, MCTSNode] - [Child nodes by action]
- visit_count: int - [Number of visits to this node]
- total_value: float - [Sum of all values from this node]
- prior_probability: float - [Prior probability from neural network]

**Public Interface**:
```python
def select_child(self, exploration_constant: float) -> 'MCTSNode':
    # Select child using UCT formula

def expand(self, action: int, prior_prob: float) -> 'MCTSNode':
    # Expand node with new child

def update(self, value: float):
    # Update node statistics

def get_visit_counts(self) -> Dict[int, int]:
    # Get visit counts for all children
```

**Lifecycle**: 
- Creation: Created during tree expansion
- Usage: Modified during search and backpropagation
- Cleanup: Automatic garbage collection

**Dependencies**: 
- GameState for state representation
- UCT formula for selection

**Usage Patterns**: 
- Created during tree expansion
- Selected during tree traversal
- Updated during backpropagation

### `mcts/core/game_state.py` - Game State Representation

#### File Purpose
Contains the game state representation and management for MCTS search.

#### Classes

**Name**: `GameState`
**Responsibility**: Represents the current state of a Yu-Gi-Oh! game
**State Schema**:
- board: np.ndarray - [Game board representation]
- player: int - [Current player (0 or 1)]
- legal_actions: List[int] - [Available legal actions]
- game_over: bool - [Whether game is finished]
- winner: Optional[int] - [Winner player if game over]

**Public Interface**:
```python
def get_legal_actions(self) -> List[int]:
    # Return list of legal actions

def apply_action(self, action: int) -> 'GameState':
    # Apply action and return new state

def is_terminal(self) -> bool:
    # Check if state is terminal

def get_reward(self) -> float:
    # Get reward for terminal state
```

**Lifecycle**: 
- Creation: Instantiated with initial game state
- Usage: Modified through action application
- Cleanup: Automatic garbage collection

**Dependencies**: 
- numpy for board representation
- Game rules for state transitions

**Usage Patterns**: 
- Created from game environment
- Modified by MCTS search
- Evaluated by neural networks

### `mcts/alphazero/alphazero.py` - AlphaZero Implementation

#### File Purpose
Contains the AlphaZero agent implementation combining MCTS with neural network evaluation.

#### Functions

**Name**: `alphazero_agent`
**Purpose**: AlphaZero-style agent using MCTS with neural network evaluation
**Input Schema**:
- state: GameState - [Current game state]
- model: NeuralNetwork - [Neural network for state evaluation]
- num_simulations: int - [Number of MCTS simulations]

**Output Schema**:
- Return Value: int - [Selected action]

**Side Effects**: 
- Model: Neural network inference calls
- Memory: MCTS tree allocation
- CPU: Search computation

**Error Conditions**: 
- ValueError: Invalid state or model
- RuntimeError: Model inference failures
- MemoryError: Insufficient memory for search

**Business Rules**: 
- Uses neural network for leaf evaluation
- Combines MCTS with policy/value networks
- Applies temperature scaling for exploration
- Supports self-play training

**Performance Notes**: 
- Neural network inference overhead
- MCTS search time dominates
- Memory usage for search tree

#### Classes

**Name**: `AlphaZeroAgent`
**Responsibility**: Complete AlphaZero agent with MCTS and neural networks
**State Schema**:
- model: NeuralNetwork - [Policy and value network]
- num_simulations: int - [Number of MCTS simulations]
- exploration_constant: float - [UCT exploration constant]
- temperature: float - [Temperature for action selection]

**Public Interface**:
```python
def __init__(self, model: NeuralNetwork, num_simulations: int = 800, exploration_constant: float = 1.0):
    # Initialize AlphaZero agent

def select_action(self, state: GameState) -> int:
    # Select action using MCTS

def get_action_probabilities(self, state: GameState) -> Dict[int, float]:
    # Get action probabilities from MCTS

def train_step(self, states: List[GameState], target_policies: List[Dict[int, float]], target_values: List[float]):
    # Train neural network
```

**Lifecycle**: 
- Creation: Instantiated with neural network
- Usage: Used for action selection and training
- Cleanup: Automatic garbage collection

**Dependencies**: 
- NeuralNetwork for evaluation
- MCTS for search
- GameState for state representation

**Usage Patterns**: 
- Created with pre-trained model
- Used for game play
- Trained through self-play

### `mcts/alphazero/neural_network.py` - Neural Network Implementation

#### File Purpose
Contains neural network implementations for AlphaZero state evaluation and policy prediction.

#### Classes

**Name**: `NeuralNetwork`
**Responsibility**: Neural network for state evaluation and policy prediction
**State Schema**:
- policy_head: nn.Module - [Policy prediction head]
- value_head: nn.Module - [Value prediction head]
- feature_extractor: nn.Module - [Feature extraction layers]

**Public Interface**:
```python
def __init__(self, policy_head: nn.Module, value_head: nn.Module, feature_extractor: nn.Module):
    # Initialize neural network

def forward(self, state: GameState) -> Tuple[Dict[int, float], float]:
    # Forward pass returning policy and value

def get_policy(self, state: GameState) -> Dict[int, float]:
    # Get policy predictions

def get_value(self, state: GameState) -> float:
    # Get value prediction
```

**Lifecycle**: 
- Creation: Instantiated with network components
- Usage: Evaluates game states
- Cleanup: Automatic garbage collection

**Dependencies**: 
- torch.nn for neural network components
- GameState for input representation

**Usage Patterns**: 
- Created with pre-trained weights
- Used for MCTS leaf evaluation
- Trained through self-play data

### `mcts/alphazero/cnode.cpp` - C++ MCTS Node Implementation

#### File Purpose
High-performance C++ implementation of MCTS nodes for improved search performance.

#### Classes

**Name**: `CNode`
**Responsibility**: High-performance MCTS node implementation in C++
**State Schema**:
- state: GameState* - [Pointer to game state]
- parent: CNode* - [Parent node pointer]
- children: std::vector<CNode*> - [Child node pointers]
- visit_count: int - [Visit count]
- total_value: double - [Total value]
- prior_prob: double - [Prior probability]

**Public Interface**:
```cpp
CNode(GameState* state, double prior_prob);
void update(double value);
CNode* select_child(double exploration_constant);
CNode* expand(int action, double prior_prob);
std::vector<int> get_visit_counts();
```

**Lifecycle**: 
- Creation: Allocated with game state
- Usage: Modified during search
- Cleanup: Manual memory management

**Dependencies**: 
- GameState for state representation
- C++ standard library for data structures

**Usage Patterns**: 
- Created during tree expansion
- Used for high-performance search
- Manually managed memory

### `mcts/alphazero/tree.cpp` - C++ MCTS Tree Implementation

#### File Purpose
High-performance C++ implementation of MCTS tree search algorithms.

#### Functions

**Name**: `mcts_search`
**Purpose**: High-performance MCTS search implementation
**Input Schema**:
- root_node: CNode* - [Root node of search tree]
- num_simulations: int - [Number of simulations]
- exploration_constant: double - [UCT exploration constant]

**Output Schema**:
- Return Value: std::vector<double> - [Action probabilities]

**Side Effects**: 
- Memory: Allocates and modifies tree nodes
- CPU: Intensive search computation

**Error Conditions**: 
- MemoryError: Insufficient memory
- RuntimeError: Search computation errors

**Business Rules**: 
- Uses UCT selection formula
- Performs random rollouts
- Backpropagates results
- Returns visit-based probabilities

**Performance Notes**: 
- Optimized C++ implementation
- Reduced memory overhead
- Faster tree operations
- Better cache locality

## Dependencies Map

### Imports Analysis

Internal Dependencies:
- mcts.mcts.core from mcts.mcts - [Purpose: Core MCTS implementation]
- mcts.mcts.alphazero from mcts.mcts - [Purpose: AlphaZero implementation]

External Dependencies:
- torch - [Purpose: Neural network framework]
- torch.nn - [Purpose: Neural network components]
- numpy - [Purpose: Numerical operations]
- numba - [Purpose: JIT compilation for performance]

### Exports Analysis

What this directory exposes and why:
- setup.py - [Package setup and configuration]
- mcts/ - [Core MCTS implementation directory]
- mcts/core/ - [Core MCTS algorithms and utilities]
- mcts/alphazero/ - [AlphaZero agent implementation]
- mcts/alphazero/alphazero.py - [Python AlphaZero implementation]
- mcts/alphazero/cnode.cpp - [C++ MCTS node implementation]
- mcts/alphazero/tree.cpp - [C++ MCTS tree implementation]

## Integration Points

- **Called by**: scripts/ for training orchestration, ygoinf/ for inference
- **Calls**: ygoai/ for game constants and utilities, ygoenv/ for environments
- **Data dependencies**: Game state representations, neural network models, training data
- **Event producers**: MCTS search events, self-play game events, training events
- **Event consumers**: Training scripts for workflow coordination, evaluation for performance measurement

## Complexity Indicators

- **Cognitive load**: High - [Reasoning: Complex MCTS algorithms with neural networks]
- **Change frequency**: Low - [How often MCTS components are modified]
- **Test coverage**: Medium - [Estimated coverage of MCTS testing]
- **Performance sensitivity**: High - [Whether performance is critical for search efficiency]
- **Error impact**: Medium - [Blast radius if MCTS fails - alternative to RL] 