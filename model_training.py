# Import necessary libraries for data processing and machine Learning

import pandas as pd
import numpy as np
from sklearn.model_selection 
import train_test_split
from sklearn.ensemble 
import RandomForestClassifier
from sklearn.metrics 
import accuracy_score, classification_report
from joblib import dump
from micromlgen import port
from joblib import load
 
# Function to read and preprocess data from multiple CSV files
def read_data(file_paths):
    data = []
    labels = []
    for i, file_path in enumerate(file_paths):
        df = pd.read_csv(file_path)
        data.append(df[['AcX', 'AcY', 'AcZ']].values)
        labels.extend([i] * len(df))
    return np.vstack(data), np.array(labels)
 
# List of training and testing file paths
train_files = ['train_position_1.csv', 'train_position_2.csv', 'train_position_3.csv', 'Position3fornewdata.csv']
test_files = ['test_position_1.csv', 'test_position_2.csv', 'test_position_3.csv', 'test_position_4.csv']
 
# Read and preprocess data using the read_data function
X_train, y_train = read_data(train_files)
X_test, y_test = read_data(test_files)
 
# Create, train, and evaluate a RandomForestClassifier model
clf = RandomForestClassifier(n_estimators=100, random_state=42)
clf.fit(X_train, y_train)
y_pred = clf.predict(X_test)
accuracy = accuracy_score(y_test, y_pred)
print("Accuracy:", accuracy)
 
# Print the classification report
print("Classification Report:")
print(classification_report(y_test, y_pred, target_names=['position_1', 'position_2', 'position_3', 'position_4']))
 
# Save the trained model to a file using joblib
dump(clf, 'trained_model.joblib')
 
# Load the saved model and convert it to C code using the micromlgen library
with open('trained_model.joblib', 'rb') as f:
    clf = load(f)
c_code = port(clf, classmap={0: 'position_1', 1: 'position_2', 2: 'position_3', 3: 'position_4'})
 
# Save the generated C code to a header file
with open('scktmodel.h', 'w') as f:
    f.write(c_code)