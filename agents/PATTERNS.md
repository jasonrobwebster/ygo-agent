# Cross-Module Design Patterns

## System-Wide Patterns

### Actor-Learner Architecture Pattern
**Purpose**: Enable distributed training with parallel environment execution
**Implementation**: 
- `scripts/cleanba.py` implements actor-learner separation
- Actors run environments and collect experience
- Learners update model parameters
- Shared parameter server for synchronization

**Usage**: 
- Training scripts use this pattern for scalability
- Multiple GPU support through device distribution
- Queue-based communication between actors and learners

### JAX-First Computation Pattern
**Purpose**: Leverage JAX's performance and compilation benefits
**Implementation**:
- All neural network code uses JAX/Flax
- JIT compilation for critical paths
- Device placement for GPU/CPU optimization
- Functional programming style for JAX compatibility

**Usage**:
- `ygoai/rl/jax/` modules follow this pattern
- Training and inference use JAX computation
- Model serialization through Flax checkpoints

### Environment Pool Pattern
**Purpose**: Efficient parallel environment execution
**Implementation**:
- `ygoenv/` provides gym-compatible interface
- envpool for high-performance environment pooling
- Parallel step execution with shared state
- Efficient memory management for large batches

**Usage**:
- Training scripts use environment pools
- Evaluation uses parallel environment execution
- Battle scripts use environment pools for agent comparison

### Stateful Inference Pattern
**Purpose**: Maintain temporal consistency in inference
**Implementation**:
- `ygoinf/` maintains duel state across requests
- RNN state persistence for temporal modeling
- Action history tracking for state reconstruction
- State expiration for memory management

**Usage**:
- Inference server uses this pattern
- Client sessions maintain state consistency
- Temporal modeling for game dynamics

## Neural Network Patterns

### RNN-Transformer Hybrid Pattern
**Purpose**: Combine temporal modeling with attention mechanisms
**Implementation**:
- `ygoai/rl/jax/agent.py` implements RNNAgent
- RNN layers for temporal state management
- Transformer layers for attention-based processing
- FiLM conditioning for feature modulation

**Usage**:
- All agent implementations use this pattern
- Configurable RNN types (LSTM, GRU, RWKV)
- Transformer depth and width configurable

### Multi-Head Attention Pattern
**Purpose**: Process different aspects of game state
**Implementation**:
- `ygoai/rl/jax/transformer.py` implements attention layers
- Separate attention heads for cards, actions, global state
- Masked attention for valid action filtering
- Positional encoding for sequence modeling

**Usage**:
- Card encoder uses attention for card relationships
- Action encoder uses attention for action sequences
- Global encoder uses attention for game state

### Feature Encoding Pattern
**Purpose**: Transform raw game state into neural features
**Implementation**:
- `ygoai/rl/jax/modules.py` implements encoding modules
- CardEncoder for individual card features
- ActionEncoder for action sequence features
- GlobalEncoder for game-wide features

**Usage**:
- All agent implementations use this pattern
- Consistent feature representation across models
- Modular design for feature composition

## Training Patterns

### CleanBA Algorithm Pattern
**Purpose**: Implement Clean Implementation of Bad Actor algorithm
**Implementation**:
- `scripts/cleanba.py` implements the training algorithm
- Actor-critic architecture with value function
- GAE advantage estimation
- PPO-style policy updates

**Usage**:
- Main training script uses this pattern
- Configurable hyperparameters for algorithm tuning
- Distributed training support

### Experience Buffer Pattern
**Purpose**: Efficient storage and sampling of training data
**Implementation**:
- `ygoai/rl/buffer.py` implements experience buffers
- Ring buffer for memory efficiency
- Batch sampling for training updates
- Transition storage with metadata

**Usage**:
- Training scripts use experience buffers
- Efficient memory management for large datasets
- Configurable buffer sizes and sampling strategies

### Checkpoint Management Pattern
**Purpose**: Reliable model persistence and recovery
**Implementation**:
- `ygoai/rl/ckpt.py` implements checkpoint utilities
- Flax-based model serialization
- Metadata storage for training state
- Automatic checkpoint rotation

**Usage**:
- Training scripts save checkpoints regularly
- Evaluation scripts load checkpoints
- Inference server loads checkpoints for deployment

## API Patterns

### FastAPI Service Pattern
**Purpose**: Provide HTTP API for model inference
**Implementation**:
- `ygoinf/server.py` implements FastAPI server
- RESTful endpoints for predictions
- Pydantic models for request/response validation
- CORS support for web clients

**Usage**:
- Inference server uses this pattern
- Client integration through HTTP API
- Automatic API documentation generation

### Stateful Session Pattern
**Purpose**: Maintain client session state across requests
**Implementation**:
- `ygoinf/models.py` implements session management
- PredictState for duel state tracking
- Action history for state reconstruction
- Session expiration for resource management

**Usage**:
- Inference server maintains session state
- Client sessions persist across game turns
- State consistency for temporal modeling

### Error Handling Pattern
**Purpose**: Graceful error handling and client communication
**Implementation**:
- FastAPI exception handlers
- Structured error responses
- Validation error handling
- Graceful degradation for service issues

**Usage**:
- API endpoints use consistent error handling
- Client applications receive meaningful error messages
- Service reliability through error recovery

## Integration Patterns

### YGOPro Compatibility Pattern
**Purpose**: Interface with YGOPro game clients
**Implementation**:
- Game state format compatibility
- Action format compatibility
- Protocol compatibility for client communication
- Replay file generation

**Usage**:
- Inference server compatible with YGOPro clients
- Battle scripts generate compatible replay files
- Client integration through standard protocols

### Environment Integration Pattern
**Purpose**: Seamless integration with game environment
**Implementation**:
- Gym-compatible environment interface
- Observation and action space definitions
- Reward function implementation
- Episode termination handling

**Usage**:
- Training scripts integrate with environment
- Evaluation scripts use environment for testing
- Battle scripts use environment for agent comparison

### Model Serialization Pattern
**Purpose**: Reliable model storage and loading
**Implementation**:
- Flax checkpoint format
- TensorFlow Lite for mobile deployment
- Model versioning and compatibility
- Parameter validation on loading

**Usage**:
- Training scripts save models in Flax format
- Inference server loads models for deployment
- Model distribution through checkpoint files

## Performance Patterns

### JIT Compilation Pattern
**Purpose**: Optimize critical computation paths
**Implementation**:
- JAX JIT compilation for model inference
- JIT compilation for training updates
- Device placement optimization
- Memory-efficient computation

**Usage**:
- All JAX-based computation uses JIT compilation
- Training and inference benefit from compilation
- Performance optimization through compilation

### Batch Processing Pattern
**Purpose**: Efficient processing of multiple samples
**Implementation**:
- Vectorized operations for batch processing
- Parallel environment execution
- Batch inference for multiple requests
- Memory-efficient batch handling

**Usage**:
- Training uses batch processing for efficiency
- Inference server processes batches when possible
- Environment pools use batch processing

### Memory Management Pattern
**Purpose**: Efficient memory usage and garbage collection
**Implementation**:
- JAX memory management
- Environment state cleanup
- Session state expiration
- Checkpoint rotation for storage management

**Usage**:
- Training scripts manage memory efficiently
- Inference server manages session memory
- Environment pools manage state memory

## Testing Patterns

### Environment Testing Pattern
**Purpose**: Validate environment correctness
**Implementation**:
- Unit tests for environment components
- Integration tests for environment workflows
- Performance tests for environment efficiency
- Regression tests for environment changes

**Usage**:
- Environment changes require testing
- Performance regression detection
- Correctness validation for game rules

### Model Testing Pattern
**Purpose**: Validate model correctness and performance
**Implementation**:
- Unit tests for model components
- Integration tests for model workflows
- Performance tests for model inference
- Regression tests for model changes

**Usage**:
- Model changes require testing
- Performance regression detection
- Correctness validation for predictions

### API Testing Pattern
**Purpose**: Validate API correctness and performance
**Implementation**:
- Unit tests for API endpoints
- Integration tests for API workflows
- Performance tests for API response times
- Load tests for API scalability

**Usage**:
- API changes require testing
- Performance regression detection
- Correctness validation for client integration 