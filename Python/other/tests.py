import tensorflow as tf
from networks.lstm import LSTMModel
import numpy as np

tf.random.set_seed(1000)

# Testing if StackedLSTM model working fine (giving same results as keras model)
def test1():
    # LSTM model from tensorflow
    lstm = tf.keras.layers.LSTMCell(units=4, kernel_initializer='random_normal', recurrent_initializer='random_normal', bias_initializer='random_normal', use_bias=True)
    dense = tf.keras.layers.Dense(units=2, kernel_initializer='random_normal', bias_initializer='random_normal', use_bias=True, activation='tanh')
    lstm.build(15)
    dense.build(4)

    # My stacked LSTM model
    model = LSTMModel(15, 4, 2, 2, True)
    model_weights = ([], [])

    w_lstm = lstm.get_weights()
    model_weights[0].append(tf.expand_dims(w_lstm[0], 0).numpy())
    model_weights[0].append(tf.expand_dims(w_lstm[1], 0).numpy())
    model_weights[1].append(tf.expand_dims(tf.expand_dims(w_lstm[2], 0), 0).numpy())

    w_dense = dense.get_weights()
    model_weights[0].append(tf.expand_dims(w_dense[0], 0).numpy())
    model_weights[1].append(tf.expand_dims(tf.expand_dims(w_dense[1], 0), 0).numpy())

    for i in range(len(model_weights)):
        for j in range(len(model_weights[i])):
            t = model_weights[i][j]
            model_weights[i][j] = np.vstack([t,t])

    model.build(model_weights=model_weights)



    states = [tf.zeros((1, 4)), tf.zeros((1, 4))]
    for i in range(5):
        # Generate Random Input
        inputs = np.random.rand(1, 15).astype(np.float32)

        # Process keras model
        output1, states = lstm(inputs, states=states)
        output1 = dense(output1)

        # Process my model
        inputs = tf.expand_dims(inputs, 0)
        inputs = np.vstack([inputs, inputs])
        output2 = model(inputs)

    print(output1)
    print(output2)

# Testing if its possible to process observations with multiple environments with single model (each environment have same agents)
def test2():
    w1 = np.random.rand(3, 5, 16).astype(np.float32)
    # w1 = np.vstack([w1, w1, w1])
    w2 = np.random.rand(3, 4, 16).astype(np.float32)
    # w2 = np.vstack([w2, w2, w2])
    w3 = np.random.rand(3, 4, 2).astype(np.float32)
    # w3 = np.vstack([w3, w3, w3])
    b1 = np.random.rand(3, 1, 16).astype(np.float32)
    # b1 = np.vstack([b1, b1, b1])
    b2 = np.random.rand(3, 1, 2).astype(np.float32)
    # b2 = np.vstack([b2, b2, b2])

    m1 = LSTMModel(5, 4, 2, 3, 1)
    m1.build(([w1, w2, w3], [b1, b2]))
    m2 = LSTMModel(5, 4, 2, 3, 1)
    m2.build(([w1, w2, w3], [b1, b2]))
    m3 = LSTMModel(5, 4, 2, 3, 1)
    m3.build(([w1, w2, w3], [b1, b2]))
    m0 = LSTMModel(5, 4, 2, 3, 3)
    m0.build(([w1, w2, w3], [b1, b2]))

    for i in range(15):
        i1 = np.random.rand(3, 1, 5).astype(np.float32)
        i2 = np.random.rand(3, 1, 5).astype(np.float32)
        i3 = np.random.rand(3, 1, 5).astype(np.float32)
        i0 = np.hstack([i1, i2, i3])

        o1 = m1(i1)
        o2 = m2(i2)
        o3 = m3(i3)
        o0 = m0(i0)

    print(o1)
    print(o2)
    print(o3)
    print(o0)

test2()