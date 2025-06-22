# YGOAI/RL Directory Context Documentation

## Purpose Statement
The ygoai/rl directory contains the reinforcement learning framework implementation for the YGO Agent system, providing PPO training algorithms, neural network agents, experience buffers, and training utilities. This is the non-JAX alternative to the JAX-based implementation in ygoai/rl/jax/.

## Core Abstractions

### `ppo.py` - PPO Training Implementation

#### File Purpose
Contains the core PPO (Proximal Policy Optimization) training algorithm implementation, including training steps, advantage computation, and entropy calculations.

#### Functions

**Name**: `entropy_from_logits(logits)`
**Purpose**: Calculates entropy from logits using softmax probability distribution
**Input Schema**:
- logits: torch.Tensor - [Action logits from neural network]

**Output Schema**:
- Return Value: torch.Tensor - [Entropy values for each sample]

**Side Effects**: 
- None - pure function

**Error Conditions**: 
- ValueError: Invalid logits tensor shape
- RuntimeError: CUDA memory errors

**Business Rules**: 
- Clamps logits to prevent numerical instability
- Uses softmax to convert logits to probabilities
- Calculates entropy as -sum(p * log(p))

**Performance Notes**: 
- Linear time complexity O(n) where n is action space size
- Memory usage scales with batch size and action space
- CUDA acceleration when available

**Name**: `train_step(agent, optimizer, scaler, mb_obs, mb_actions, mb_logprobs, mb_advantages, mb_returns, mb_values, mb_learns, args)`
**Purpose**: Performs a single PPO training step with policy and value function updates
**Input Schema**:
- agent: PPOAgent - [Neural network agent]
- optimizer: torch.optim.Optimizer - [Optimizer for parameter updates]
- scaler: Optional[torch.cuda.amp.GradScaler] - [Gradient scaler for mixed precision]
- mb_obs: torch.Tensor - [Mini-batch observations]
- mb_actions: torch.Tensor - [Mini-batch actions taken]
- mb_logprobs: torch.Tensor - [Mini-batch log probabilities]
- mb_advantages: torch.Tensor - [Mini-batch advantage estimates]
- mb_returns: torch.Tensor - [Mini-batch returns]
- mb_values: torch.Tensor - [Mini-batch value estimates]
- mb_learns: torch.Tensor - [Mini-batch learning masks]
- args: Namespace - [Training configuration arguments]

**Output Schema**:
- Return Value: Tuple[torch.Tensor, torch.Tensor, torch.Tensor, torch.Tensor, torch.Tensor, torch.Tensor] - [old_approx_kl, approx_kl, clipfrac, pg_loss, v_loss, entropy_loss]

**Side Effects**: 
- Model: Updates agent parameters through backpropagation
- Memory: Allocates gradients and intermediate tensors
- CUDA: GPU memory operations if using CUDA

**Error Conditions**: 
- RuntimeError: CUDA out of memory
- ValueError: Invalid tensor shapes
- AssertionError: Invalid training arguments

**Business Rules**: 
- Uses PPO clipping for policy loss
- Optional value function clipping
- Advantage normalization if enabled
- Entropy regularization for exploration

**Performance Notes**: 
- Forward pass: O(batch_size * model_size)
- Backward pass: O(batch_size * model_size)
- Memory usage scales with batch size and model size

**Name**: `bootstrap_value(values, rewards, dones, nextvalues, next_done, gamma, gae_lambda)`
**Purpose**: Computes Generalized Advantage Estimation (GAE) for value function training
**Input Schema**:
- values: torch.Tensor - [Value estimates for each timestep]
- rewards: torch.Tensor - [Rewards for each timestep]
- dones: torch.Tensor - [Episode termination flags]
- nextvalues: torch.Tensor - [Value estimates for next timestep]
- next_done: torch.Tensor - [Next episode termination flag]
- gamma: float - [Discount factor]
- gae_lambda: float - [GAE lambda parameter]

**Output Schema**:
- Return Value: torch.Tensor - [Computed advantages]

**Side Effects**: 
- None - pure function

**Error Conditions**: 
- ValueError: Invalid tensor shapes or parameters
- RuntimeError: CUDA memory errors

**Business Rules**: 
- Handles episode boundaries correctly
- Uses GAE formula for advantage computation
- Supports both single and multi-environment batches

**Performance Notes**: 
- Linear time complexity O(n) where n is sequence length
- Memory usage scales with sequence length
- CUDA acceleration when available

**Name**: `bootstrap_value_selfplay(values, rewards, dones, learns, nextvalues1, nextvalues2, next_done, gamma, gae_lambda)`
**Purpose**: Computes GAE for self-play scenarios with two players
**Input Schema**:
- values: torch.Tensor - [Value estimates for each timestep]
- rewards: torch.Tensor - [Rewards for each timestep]
- dones: torch.Tensor - [Episode termination flags]
- learns: torch.Tensor - [Learning player indicators]
- nextvalues1: torch.Tensor - [Player 1 next value estimates]
- nextvalues2: torch.Tensor - [Player 2 next value estimates]
- next_done: torch.Tensor - [Next episode termination flag]
- gamma: float - [Discount factor]
- gae_lambda: float - [GAE lambda parameter]

**Output Schema**:
- Return Value: torch.Tensor - [Computed advantages for self-play]

**Side Effects**: 
- None - pure function

**Error Conditions**: 
- ValueError: Invalid tensor shapes or parameters
- RuntimeError: CUDA memory errors

**Business Rules**: 
- Handles player switching in self-play
- Maintains separate value estimates for each player
- Correctly handles episode boundaries

**Performance Notes**: 
- Linear time complexity O(n) where n is sequence length
- Memory usage scales with sequence length
- CUDA acceleration when available

### `agent.py` - Neural Network Agent Implementation

#### File Purpose
Contains the core neural network agent implementations including encoders, actors, and the complete PPO agent.

#### Classes

**Name**: `PositionalEncoding`
**Responsibility**: Adds positional information to transformer embeddings
**State Schema**:
- dropout: nn.Dropout - [Dropout layer]
- pe: torch.Tensor - [Pre-computed positional encodings]

**Public Interface**:
```python
def __init__(self, d_model: int, dropout: float = 0.0, max_len: int = 5000):
    # Initialize positional encoding

def forward(self, x: torch.Tensor) -> torch.Tensor:
    # Add positional encoding to input
```

**Lifecycle**: 
- Creation: Instantiated with model dimension and parameters
- Usage: Applied to transformer inputs
- Cleanup: Automatic garbage collection

**Dependencies**: 
- torch.nn for neural network components
- math for mathematical operations

**Usage Patterns**: 
- Used in transformer architectures
- Applied to sequence inputs
- Supports variable sequence lengths

**Name**: `Encoder`
**Responsibility**: Encodes Yu-Gi-Oh! game state into neural network representations
**State Schema**:
- channels: int - [Number of feature channels]
- loc_embed: nn.Embedding - [Location embeddings]
- seq_embed: nn.Embedding - [Sequence embeddings]
- id_embed: nn.Embedding - [Card ID embeddings]
- card_net: nn.ModuleList - [Card transformer layers]
- action_card_net: nn.ModuleList - [Action card transformer layers]
- history_action_net: nn.ModuleList - [Action history transformer layers]

**Public Interface**:
```python
def __init__(self, channels=128, num_card_layers=2, num_action_layers=2, embedding_shape=None, bias=False, affine=True):
    # Initialize encoder with configuration

def forward(self, x: Dict[str, torch.Tensor]) -> Tuple[torch.Tensor, torch.Tensor, torch.Tensor]:
    # Encode game state to features, actions, and validity
```

**Lifecycle**: 
- Creation: Instantiated with architecture configuration
- Usage: Processes game states during training and inference
- Cleanup: Automatic garbage collection

**Dependencies**: 
- torch.nn for neural network components
- ygoai for card embeddings and constants

**Usage Patterns**: 
- Core component of PPOAgent
- Processes complex game state dictionaries
- Outputs features for policy and value heads

**Name**: `Actor`
**Responsibility**: Implements the policy network for action selection
**State Schema**:
- channels: int - [Number of input channels]
- use_transformer: bool - [Whether to use transformer architecture]
- layers: nn.ModuleList - [Neural network layers]

**Public Interface**:
```python
def __init__(self, channels, use_transformer=False):
    # Initialize actor network

def forward(self, f_actions: torch.Tensor, mask: torch.Tensor) -> torch.Tensor:
    # Compute action logits
```

**Lifecycle**: 
- Creation: Instantiated with channel configuration
- Usage: Generates action probabilities
- Cleanup: Automatic garbage collection

**Dependencies**: 
- torch.nn for neural network components
- Encoder for feature extraction

**Usage Patterns**: 
- Component of PPOAgent
- Processes encoded features
- Outputs action logits with masking

**Name**: `PPOAgent`
**Responsibility**: Complete PPO agent with encoder, actor, and value function
**State Schema**:
- encoder: Encoder - [Game state encoder]
- actor: Actor - [Policy network]
- value_head: nn.Linear - [Value function head]

**Public Interface**:
```python
def __init__(self, channels=128, num_card_layers=2, num_action_layers=2, embedding_shape=None, bias=False, affine=True, a_trans=True):
    # Initialize PPO agent

def forward(self, x: Dict[str, torch.Tensor]) -> Tuple[torch.Tensor, torch.Tensor, torch.Tensor]:
    # Forward pass returning logits, value, and validity

def get_logit(self, x: Dict[str, torch.Tensor]) -> torch.Tensor:
    # Get action logits only

def get_value(self, x: Dict[str, torch.Tensor]) -> torch.Tensor:
    # Get value estimate only
```

**Lifecycle**: 
- Creation: Instantiated with architecture configuration
- Usage: Main agent for training and inference
- Cleanup: Automatic garbage collection

**Dependencies**: 
- Encoder for state encoding
- Actor for policy computation
- torch.nn for value function

**Usage Patterns**: 
- Primary agent in training scripts
- Used for both training and evaluation
- Supports embedding loading and freezing

### `buffer.py` - Experience Buffer Implementation

#### File Purpose
Contains experience buffer implementations for storing and managing training data during PPO training.

#### Data Structures/Types

**Name**: `ExperienceBuffer`
**Purpose**: Stores training experiences for PPO updates
**Schema Definition**:

Required Fields:
- observations: torch.Tensor - [Game state observations]
- actions: torch.Tensor - [Actions taken]
- logprobs: torch.Tensor - [Log probabilities of actions]
- advantages: torch.Tensor - [Computed advantages]
- returns: torch.Tensor - [Computed returns]
- values: torch.Tensor - [Value estimates]

Optional Fields:
- learns: torch.Tensor - [Learning player indicators]
- dones: torch.Tensor - [Episode termination flags]

**Validation Rules**: 
- All tensors must have same batch dimension
- Advantages must be computed before use
- Returns must be computed before use

**Transformation Patterns**: 
- Raw experiences → Buffer storage
- Buffer → Mini-batches
- Mini-batches → Training updates

**Persistence**: 
- Lives for training episode
- Cleared after each update
- Memory efficient storage

### `ckpt.py` - Checkpoint Utilities

#### File Purpose
Contains utilities for saving and loading model checkpoints during training.

#### Functions

**Name**: `save_checkpoint(model, optimizer, epoch, filename)`
**Purpose**: Saves model state, optimizer state, and training metadata to checkpoint file
**Input Schema**:
- model: nn.Module - [Neural network model]
- optimizer: torch.optim.Optimizer - [Optimizer state]
- epoch: int - [Current training epoch]
- filename: str - [Checkpoint file path]

**Output Schema**:
- Return Value: bool - [Save success status]

**Side Effects**: 
- File system: Creates checkpoint file
- Memory: Serializes model and optimizer state

**Error Conditions**: 
- IOError: File system errors
- RuntimeError: Model serialization errors

**Business Rules**: 
- Saves complete model state
- Includes optimizer state for resuming training
- Stores training metadata

**Performance Notes**: 
- File I/O is primary bottleneck
- Memory usage scales with model size
- Supports compression for large models

### `dist.py` - Distribution Utilities

#### File Purpose
Contains utilities for distributed training and multi-GPU operations.

#### Functions

**Name**: `setup_distributed_training(world_size, rank)`
**Purpose**: Sets up distributed training environment
**Input Schema**:
- world_size: int - [Total number of processes]
- rank: int - [Process rank]

**Output Schema**:
- Return Value: bool - [Setup success status]

**Side Effects**: 
- Network: Initializes distributed communication
- Process: Sets up process groups

**Error Conditions**: 
- RuntimeError: Communication setup failures
- ValueError: Invalid world size or rank

**Business Rules**: 
- Initializes process groups
- Sets up communication backends
- Configures device placement

**Performance Notes**: 
- Network initialization overhead
- Memory usage for communication buffers
- Scales with number of processes

### `utils.py` - RL Utility Functions

#### File Purpose
Contains utility functions for reinforcement learning operations.

#### Functions

**Name**: `masked_normalize(tensor, mask, eps=1e-8)`
**Purpose**: Normalizes tensor values using masked mean and standard deviation
**Input Schema**:
- tensor: torch.Tensor - [Input tensor to normalize]
- mask: torch.Tensor - [Boolean mask for valid values]
- eps: float - [Small value to prevent division by zero]

**Output Schema**:
- Return Value: torch.Tensor - [Normalized tensor]

**Side Effects**: 
- None - pure function

**Error Conditions**: 
- ValueError: Invalid tensor shapes
- RuntimeError: CUDA memory errors

**Business Rules**: 
- Computes masked mean and std
- Applies normalization only to masked values
- Handles edge cases with small eps

**Performance Notes**: 
- Linear time complexity
- Memory usage scales with tensor size
- CUDA acceleration when available

**Name**: `masked_mean(tensor, mask)`
**Purpose**: Computes mean of tensor values using boolean mask
**Input Schema**:
- tensor: torch.Tensor - [Input tensor]
- mask: torch.Tensor - [Boolean mask for valid values]

**Output Schema**:
- Return Value: torch.Tensor - [Masked mean value]

**Side Effects**: 
- None - pure function

**Error Conditions**: 
- ValueError: Invalid tensor shapes
- RuntimeError: CUDA memory errors

**Business Rules**: 
- Sums only masked values
- Divides by number of masked values
- Handles empty mask case

**Performance Notes**: 
- Linear time complexity
- Memory efficient computation
- CUDA acceleration when available

### `env.py` - Environment Wrappers

#### File Purpose
Contains environment wrapper classes for integrating with gymnasium and custom environments.

#### Classes

**Name**: `YGOEnvWrapper`
**Responsibility**: Wraps Yu-Gi-Oh! environments for gymnasium compatibility
**State Schema**:
- env: Any - [Underlying Yu-Gi-Oh! environment]
- observation_space: gym.Space - [Observation space definition]
- action_space: gym.Space - [Action space definition]

**Public Interface**:
```python
def __init__(self, env):
    # Initialize wrapper

def reset(self) -> Tuple[Any, Dict]:
    # Reset environment

def step(self, action) -> Tuple[Any, float, bool, bool, Dict]:
    # Take environment step
```

**Lifecycle**: 
- Creation: Instantiated with underlying environment
- Usage: Provides gymnasium interface
- Cleanup: Automatic garbage collection

**Dependencies**: 
- gymnasium for interface compatibility
- ygoenv for underlying environment

**Usage Patterns**: 
- Wraps custom environments
- Provides standard gym interface
- Handles observation/action conversion

### `eval.py` - Evaluation Utilities

#### File Purpose
Contains utilities for evaluating trained agents and computing performance metrics.

#### Functions

**Name**: `evaluate_agent(agent, env, num_episodes)`
**Purpose**: Evaluates agent performance over multiple episodes
**Input Schema**:
- agent: PPOAgent - [Trained agent to evaluate]
- env: gym.Env - [Environment for evaluation]
- num_episodes: int - [Number of episodes to run]

**Output Schema**:
- Return Value: Dict[str, float] - [Performance metrics]

**Side Effects**: 
- Environment: Runs evaluation episodes
- Model: Performs inference

**Error Conditions**: 
- RuntimeError: Environment errors
- ValueError: Invalid parameters

**Business Rules**: 
- Runs specified number of episodes
- Computes win rate and average rewards
- Handles episode termination

**Performance Notes**: 
- Linear time with number of episodes
- Memory usage for episode data
- GPU inference acceleration

## Dependencies Map

### Imports Analysis

Internal Dependencies:
- ygoai.rl.utils from ygoai.rl - [Purpose: RL utility functions]
- ygoai from ygoai - [Purpose: Game constants and utilities]

External Dependencies:
- torch - [Purpose: Deep learning framework]
- torch.nn - [Purpose: Neural network components]
- torch.cuda.amp - [Purpose: Mixed precision training]
- torch_xla.core.xla_model - [Purpose: TPU support]
- numpy - [Purpose: Numerical operations]
- numba - [Purpose: JIT compilation for performance]
- gymnasium - [Purpose: Environment interface]
- psutil - [Purpose: System monitoring]

### Exports Analysis

What this directory exposes and why:
- ppo.py - [PPO training algorithms and GAE computation]
- agent.py - [Neural network agent implementations]
- agent2.py - [Alternative agent implementation]
- buffer.py - [Experience buffer implementations]
- ckpt.py - [Checkpoint utilities]
- dist.py - [Distribution utilities]
- utils.py - [RL utility functions]
- env.py - [Environment wrappers]
- eval.py - [Evaluation utilities]

## Integration Points

- **Called by**: scripts/ for training orchestration, ygoinf/ for inference
- **Calls**: ygoai/ for game constants and utilities, ygoenv/ for environments
- **Data dependencies**: Game state observations, action spaces, reward signals
- **Event producers**: Training step events, agent creation events, buffer operations
- **Event consumers**: Training scripts for workflow coordination, evaluation for performance measurement

## Complexity Indicators

- **Cognitive load**: High - [Reasoning: Complex RL algorithms with neural networks]
- **Change frequency**: Medium - [How often RL components are modified]
- **Test coverage**: Medium - [Estimated coverage of RL testing]
- **Performance sensitivity**: High - [Whether performance is critical for training efficiency]
- **Error impact**: High - [Blast radius if RL fails - training stops] 