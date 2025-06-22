# Scripts Directory Context Documentation

## Purpose Statement
The scripts directory contains the core training, evaluation, and inference scripts for the YGO Agent system. It provides the main entry points for training AI agents to play Yu-Gi-Oh! using reinforcement learning, evaluating agent performance, and deploying trained models for human-AI gameplay.

## Core Abstractions

### Functions/Methods

#### `cleanba.py` - Main Training Script

**Name**: `main()`
**Purpose**: Orchestrates the complete training pipeline for YGO agents using CleanBA (Clean Implementation of Bad Actor) algorithm
**Input Schema**:
- Command line arguments: Args dataclass - [Training configuration including environment, model, and algorithm parameters]
- Environment setup: ygoenv environment - [Yu-Gi-Oh! game environment with gym interface]
- Model initialization: RNNAgent - [Neural network agent with RNN architecture]

**Output Schema**:
- Return Value: None - [Training runs until completion or interruption]
- Side Effects: 
  - Model checkpoints saved to disk
  - TensorBoard logs written
  - Training metrics printed to console
  - Model parameters distributed to actor threads

**Side Effects**: 
- File system: Creates checkpoints and log directories
- GPU memory: Allocates and manages JAX device memory
- Network: TensorBoard logging and distributed training communication
- State mutations: Updates model parameters and training state

**Error Conditions**: 
- NaN/Inf loss values trigger training termination
- GPU memory exhaustion causes OOM errors
- Environment timeouts cause episode termination
- Invalid deck files cause initialization failures

**Business Rules**: 
- Training continues until total_timesteps reached or manual interruption
- Model saved every save_interval updates
- Evaluation performed every eval_interval updates
- Learning rate annealed according to schedule

**Performance Notes**: 
- Parallel environment execution with configurable thread count
- JAX JIT compilation for model inference and training
- Multi-GPU distributed training support
- Optimized data pipeline with queue-based actor-learner communication

#### `eval.py` - Evaluation Script

**Name**: `main()`
**Purpose**: Evaluates trained YGO agents against different strategies and measures performance metrics
**Input Schema**:
- checkpoint: str - [Path to trained model checkpoint file]
- num_episodes: int - [Number of evaluation episodes to run]
- strategy: Literal["random", "greedy"] - [Opponent strategy to evaluate against]
- num_envs: int - [Number of parallel environments for evaluation]

**Output Schema**:
- Return Value: None - [Evaluation results printed to console]
- Additional Info: dict - [Performance metrics including win rate, average game length, rewards]

**Side Effects**: 
- Model loading from checkpoint files
- Environment creation and episode execution
- Performance metrics calculation and reporting

**Error Conditions**: 
- Invalid checkpoint files cause loading failures
- Environment errors cause episode termination
- Memory issues with large batch sizes

**Business Rules**: 
- Evaluation runs for specified number of episodes
- Win rate calculated as percentage of games won
- Average game length measured in turns
- Performance metrics aggregated across all episodes

**Performance Notes**: 
- Parallel environment execution for faster evaluation
- JAX JIT compilation for model inference
- Configurable batch sizes for memory optimization

#### `battle.py` - Agent vs Agent Evaluation

**Name**: `main()`
**Purpose**: Pits two trained agents against each other to determine relative performance
**Input Schema**:
- checkpoint1: str - [Path to first agent's checkpoint]
- checkpoint2: str - [Path to second agent's checkpoint]
- num_episodes: int - [Number of battles to run]
- record: bool - [Whether to save replay files]

**Output Schema**:
- Return Value: None - [Battle results printed to console]
- Additional Info: dict - [Payoff matrix showing win rates for each agent as player 1 and 2]

**Side Effects**: 
- Replay file generation if recording enabled
- Model loading and evaluation
- Performance matrix calculation

**Error Conditions**: 
- Invalid checkpoint files
- Environment errors during battles
- File system issues with replay recording

**Business Rules**: 
- Each agent plays as both first and second player
- Win rates calculated for each position
- Payoff matrix shows relative performance
- Replay files saved in YGOPro format

**Performance Notes**: 
- Parallel battle execution
- JAX JIT compilation for model inference
- Efficient replay file generation

### Classes/Objects

#### `Args` Dataclass (cleanba.py)

**Name**: `Args`
**Responsibility**: Configuration management for training experiments
**State Schema**:
- exp_name: str - [Experiment name for logging and checkpointing]
- seed: int - [Random seed for reproducibility]
- env_id: str - [Environment identifier, defaults to "YGOPro-v1"]
- deck: str - [Path to deck file or directory]
- total_timesteps: int - [Total training timesteps, default 50B]
- learning_rate: float - [Learning rate for optimizer, default 3e-4]
- local_num_envs: int - [Number of parallel environments per actor]
- num_steps: int - [Steps per environment per rollout, default 128]
- num_minibatches: int - [Number of minibatches for training, default 64]
- update_epochs: int - [Training epochs per update, default 1]
- gamma: float - [Discount factor, default 1.0]
- gae_lambda: float - [GAE lambda parameter, default 0.95]
- ent_coef: float - [Entropy coefficient, default 0.01]
- vf_coef: float - [Value function coefficient, default 1.0]
- max_grad_norm: float - [Gradient clipping norm, default 1.0]
- actor_device_ids: List[int] - [GPU devices for actor workers]
- learner_device_ids: List[int] - [GPU devices for learner workers]

**Public Interface**:
- All fields accessible as attributes for configuration
- tyro integration for command-line argument parsing
- Validation and default value handling

**Lifecycle**: 
- Creation: Instantiated with default values
- Configuration: Modified via command-line arguments
- Usage: Passed to training functions
- Cleanup: Automatic garbage collection

**Dependencies**: 
- tyro for argument parsing
- dataclasses for structure
- typing for type hints

**Usage Patterns**: 
- Created at script startup
- Configured via command-line arguments
- Passed to all training functions
- Used for logging and checkpoint naming

#### `Transition` NamedTuple (cleanba.py)

**Name**: `Transition`
**Responsibility**: Data structure for storing training transitions
**State Schema**:
- obs: list - [Observations from environment]
- dones: list - [Episode termination flags]
- actions: list - [Actions taken by agent]
- logits: list - [Policy logits from model]
- values: list - [Value function outputs]
- rewards: list - [Rewards received from environment]
- mains: list - [Main phase indicators]
- next_dones: list - [Next step termination flags]

**Public Interface**:
- Immutable data structure
- Field access via attribute names
- Tuple unpacking support

**Lifecycle**: 
- Creation: Instantiated with transition data
- Storage: Collected in training buffers
- Processing: Batched for training updates
- Cleanup: Automatic garbage collection

**Dependencies**: 
- collections.NamedTuple
- numpy for data arrays

**Usage Patterns**: 
- Created during environment rollouts
- Stored in training buffers
- Batched for model updates
- Used for advantage computation

### Data Structures/Types

#### Training Configuration Schema

**Name**: `TrainingConfig`
**Purpose**: Complete configuration for YGO agent training
**Schema Definition**:

Required Fields:
- env_id: str - [Environment identifier, must be "YGOPro-v1"]
- deck: str - [Deck file path, must exist and be valid YDK format]
- total_timesteps: int - [Total training steps, must be positive]
- learning_rate: float - [Learning rate, must be positive and typically 1e-5 to 1e-3]
- local_num_envs: int - [Environment count, must be positive and power of 2 for efficiency]

Optional Fields:
- checkpoint: Optional[str] - [Resume training from checkpoint]
- embedding_file: Optional[str] - [Card embedding file for enhanced representation]
- eval_interval: int - [Evaluation frequency, default 100 updates]
- save_interval: int - [Checkpoint frequency, default 400 updates]
- tb_dir: Optional[str] - [TensorBoard log directory]

Computed/Derived Fields:
- batch_size: int - [Calculated as num_envs * num_steps]
- minibatch_size: int - [Calculated as batch_size // num_minibatches]
- num_updates: int - [Calculated as total_timesteps // batch_size]

**Validation Rules**: 
- All numeric fields must be positive
- Deck file must exist and be valid
- Device IDs must be valid GPU indices
- Learning rate must be in reasonable range

**Transformation Patterns**: 
- Command-line arguments → Args dataclass
- Args dataclass → Training configuration
- Configuration → Environment and model setup

**Persistence**: 
- Stored as command-line arguments
- Logged to TensorBoard
- Saved in experiment metadata

#### Model Checkpoint Schema

**Name**: `ModelCheckpoint`
**Purpose**: Serialized model state for training resumption and deployment
**Schema Definition**:

Required Fields:
- params: dict - [Model parameters organized by layer]
- opt_state: dict - [Optimizer state including momentum and learning rate]
- batch_stats: dict - [Batch normalization statistics if applicable]

Optional Fields:
- step: int - [Training step when checkpoint was saved]
- timestamp: str - [ISO timestamp of checkpoint creation]
- metrics: dict - [Training metrics at checkpoint time]

**Validation Rules**: 
- Parameters must be valid JAX arrays
- Optimizer state must match parameter structure
- File must be readable and contain valid Flax format

**Transformation Patterns**: 
- Model state → Flax serialization → File
- File → Flax deserialization → Model state
- Checkpoint → Model initialization

**Persistence**: 
- Stored as .flax_model files
- Compressed for storage efficiency
- Versioned with training step numbers

### API Endpoints/Routes

#### Training Pipeline API

**Endpoint**: `python cleanba.py [args]`
**Purpose**: Execute complete training pipeline for YGO agents
**Authentication**: None required - local execution
**Request Schema**:

Command Line Arguments:
- --deck: str - [Deck file path, required]
- --total_timesteps: int - [Training duration, default 50B]
- --learning_rate: float - [Learning rate, default 3e-4]
- --local_num_envs: int - [Environment count, default 128]
- --checkpoint: Optional[str] - [Resume from checkpoint]
- --eval_interval: int - [Evaluation frequency, default 100]
- --save_interval: int - [Checkpoint frequency, default 400]

**Response Schema**:

Success Response (Training completion):
- Console output: Training progress and metrics
- Files created: Checkpoints and TensorBoard logs
- Exit code: 0 for successful completion

Error Responses:
- 1: Invalid arguments or configuration
- 2: Environment or model initialization failure
- 3: Training error (NaN loss, OOM, etc.)

**Business Logic**: 
- Validates all input parameters
- Initializes environment and model
- Executes training loop with logging
- Handles errors and graceful shutdown

**Side Effects**: 
- Creates checkpoint and log directories
- Allocates GPU memory
- Writes training logs and checkpoints
- May use significant computational resources

**Performance**: 
- Training time: Hours to days depending on configuration
- Memory usage: Scales with batch size and model size
- GPU utilization: High during training phases

#### Evaluation API

**Endpoint**: `python eval.py [args]`
**Purpose**: Evaluate trained agent performance
**Authentication**: None required - local execution
**Request Schema**:

Command Line Arguments:
- --checkpoint: str - [Model checkpoint path, required]
- --num_episodes: int - [Evaluation episodes, default 1024]
- --strategy: str - [Opponent strategy, "random" or "greedy"]
- --num_envs: int - [Parallel environments, default 64]

**Response Schema**:

Success Response:
- Console output: Performance metrics
- Metrics: Win rate, average game length, reward statistics

Error Responses:
- 1: Invalid checkpoint or configuration
- 2: Environment initialization failure
- 3: Evaluation error

**Business Logic**: 
- Loads trained model from checkpoint
- Creates evaluation environment
- Runs specified number of episodes
- Calculates and reports performance metrics

**Side Effects**: 
- Model loading and GPU memory allocation
- Environment creation and execution
- Performance metric calculation

**Performance**: 
- Evaluation time: Minutes to hours depending on episodes
- Memory usage: Scales with batch size
- GPU utilization: Moderate during inference

### Service/Module Interfaces

#### Training Service Interface

**Service Name**: `TrainingService`
**Domain Responsibility**: Orchestrates complete training pipeline for YGO agents
**Public Interface**:

```python
class TrainingService:
    # Core operations
    def train(args: Args) -> None:  # Execute training pipeline
    def evaluate(checkpoint: str, episodes: int) -> dict:  # Evaluate model
    def battle(checkpoint1: str, checkpoint2: str, episodes: int) -> dict:  # Agent vs agent
    
    # Configuration
    def validate_config(args: Args) -> bool:  # Validate training configuration
    def setup_environment(args: Args) -> ygoenv.Env:  # Initialize game environment
    def create_agent(args: Args) -> RNNAgent:  # Initialize neural network agent
    
    # Training utilities
    def save_checkpoint(model: dict, path: str) -> None:  # Save model state
    def load_checkpoint(path: str) -> dict:  # Load model state
    def log_metrics(writer: SummaryWriter, metrics: dict, step: int) -> None:  # Log training metrics
```

**Dependencies**: 
- ygoenv for game environment
- ygoai.rl.jax.agent for neural network models
- JAX for computation
- TensorBoard for logging

**Configuration**:
```python
{
    "env_id": "YGOPro-v1",
    "deck_path": "../assets/deck",
    "learning_rate": 3e-4,
    "batch_size": 16384,
    "num_episodes": 1024,
    "checkpoint_dir": "checkpoints",
    "log_dir": "runs"
}
```

**Error Handling**: 
- Invalid configuration throws ValueError
- Environment errors trigger graceful shutdown
- Training errors logged and reported

**Monitoring**: 
- Training progress via TensorBoard
- Performance metrics tracking
- Resource utilization monitoring

### Integration/External System Interfaces

#### YGOPro Environment Integration

**System Name**: `ygoenv`
**Purpose**: Provides Yu-Gi-Oh! game environment for reinforcement learning
**Protocol**: Gymnasium-compatible environment interface
**Authentication**: None - local environment
**Request/Response Examples**:

Environment Creation:
```python
envs = ygoenv.make(
    task_id="YGOPro-v1",
    env_type="gymnasium",
    num_envs=128,
    deck1="BlueEyes.ydk",
    deck2="CyberDragon.ydk",
    max_options=24,
    n_history_actions=32
)
```

Environment Step:
```python
obs, reward, done, info = envs.step(actions)
# obs: Dict with game state observations
# reward: float reward signal
# done: bool episode termination
# info: dict with additional game information
```

**Error Handling**: 
- Invalid deck files cause initialization failure
- Game rule violations handled by environment
- Timeout errors for long games

**Rate Limits**: 
- Environment step timeouts (600s default)
- Memory limits for large batch sizes
- GPU memory constraints

**Data Mapping**: 
- Game state → Observation tensors
- Actions → Environment step calls
- Rewards → Training signal

#### TensorBoard Integration

**System Name**: `TensorBoard`
**Purpose**: Training visualization and metric tracking
**Protocol**: TensorBoard logging protocol
**Authentication**: None - local file system
**Request/Response Examples**:

Logging Setup:
```python
from torch.utils.tensorboard import SummaryWriter
writer = SummaryWriter(log_dir="runs/experiment_name")
```

Metric Logging:
```python
writer.add_scalar("losses/policy_loss", policy_loss, global_step)
writer.add_scalar("charts/win_rate", win_rate, global_step)
```

**Error Handling**: 
- File system errors for log directory
- Disk space issues for large experiments
- Permission errors for log writing

**Rate Limits**: 
- Logging frequency (every 10 updates default)
- Disk space for log files
- Memory for metric aggregation

**Data Mapping**: 
- Training metrics → TensorBoard events
- Model graphs → TensorBoard visualization
- Hyperparameters → TensorBoard config

### Business Logic/Workflow Abstractions

#### Training Workflow

**Workflow Name**: `YGOAgentTraining`
**Business Purpose**: Train AI agents to achieve superhuman performance in Yu-Gi-Oh!
**Process Steps**:

```
1. Environment Setup
   ├── Load deck configurations
   ├── Initialize YGOPro environment
   ├── Configure observation and action spaces
   └── Validate environment compatibility

2. Model Initialization
   ├── Create RNNAgent with specified architecture
   ├── Initialize model parameters
   ├── Setup optimizer and learning rate schedule
   └── Load embeddings if provided

3. Training Loop
   ├── Actor Phase: Collect experience with current policy
   ├── Learner Phase: Update model parameters
   ├── Evaluation Phase: Assess performance against baselines
   └── Checkpoint Phase: Save model state periodically

4. Completion
   ├── Save final model checkpoint
   ├── Generate training summary
   ├── Clean up resources
   └── Report final performance metrics
```

**Decision Points**: 
- Training continuation based on performance metrics
- Learning rate adjustment based on loss curves
- Early stopping based on convergence criteria
- Checkpoint frequency based on training stability

**Rollback/Compensation**: 
- Automatic checkpoint recovery on failure
- Training resumption from last valid checkpoint
- Graceful degradation with reduced batch sizes

**Business Rules**: 
- Training continues until total_timesteps reached
- Model saved every save_interval updates
- Evaluation performed every eval_interval updates
- Learning rate annealed according to schedule

**State Transitions**: 
- Initialization → Training → Evaluation → Checkpoint → Training (repeat)
- Training → Completion (when total_timesteps reached)
- Training → Error → Recovery → Training (on failure)

## Dependencies Map

### Imports Analysis

Internal Dependencies:
- ygoai.utils from ygoai - [Purpose: YGOPro initialization and utility functions]
- ygoai.rl.utils from ygoai.rl - [Purpose: RL utilities and environment wrappers]
- ygoai.rl.jax.agent from ygoai.rl.jax - [Purpose: Neural network agent implementation]
- ygoai.rl.jax.utils from ygoai.rl.jax - [Purpose: JAX-specific utilities and loss functions]

External Dependencies:
- ygoenv - [Purpose: Yu-Gi-Oh! game environment]
- jax - [Purpose: High-performance numerical computing]
- flax - [Purpose: Neural network library for JAX]
- optax - [Purpose: Optimization library for JAX]
- tyro - [Purpose: Command-line argument parsing]
- tensorboardX - [Purpose: Training visualization and logging]

### Exports Analysis

What this directory exposes and why:
- cleanba.py - [Primary training script for YGO agents]
- eval.py - [Model evaluation and performance measurement]
- battle.py - [Agent vs agent comparison and ranking]
- code_list.txt - [Supported card codes for deck validation]

## Integration Points

- **Called by**: Users and researchers for training YGO agents
- **Calls**: ygoenv for game environment, ygoai modules for agent implementation
- **Data dependencies**: Deck files, card embeddings, model checkpoints
- **Event producers**: Training logs, model checkpoints, performance metrics
- **Event consumers**: TensorBoard for visualization, evaluation scripts for assessment

## Complexity Indicators

- **Cognitive load**: High - [Reasoning: Complex RL training pipeline with multiple components]
- **Change frequency**: Medium - [How often training scripts are modified for new experiments]
- **Test coverage**: Low - [Estimated coverage of training pipeline testing]
- **Performance sensitivity**: High - [Whether performance is critical for training efficiency]
- **Error impact**: High - [Blast radius if training pipeline fails - lost computation time] 