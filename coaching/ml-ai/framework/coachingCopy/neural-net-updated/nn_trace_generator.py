#!/usr/bin/env python3
"""
nn_trace_generator.py

Fill in your weights/dataset below (or load from CSV) and run:

    python nn_trace_generator.py

It generates:
    nn_full_training_trace.txt

Features:
- Forward propagation
- Backpropagation
- Per-example uppercase deltas
- Running accumulated deltas
- Batch cost
- Weight update with optional L2 regularization

This template uses the same conventions discussed:
- Hidden activations include a bias element at index 0.
- Output delta: delta3 = a3 - y
- Hidden delta skips the bias component after W.T @ delta.
"""

import math

LEARNING_RATE = 0.01
LAMBDA = 0.01
USE_REGULARIZATION = True

def sigmoid(x):
    return 1.0/(1.0+math.exp(-x))

def dsigmoid_from_activation(a):
    return a*(1-a)

def matvec(W,v):
    return [sum(w*x for w,x in zip(r,v)) for r in W]

def outer(a,b):
    return [[x*y for y in b] for x in a]

# --------------------------------------------------------------------
# Paste your model weights here
# --------------------------------------------------------------------

W0 = [[0.606412980196915, 0.992453107031227, 0.109787136998726, 0.412290362203887, 0.722321839175809],
      [0.494228642999667, 0.731318605408058, 0.719344802878138, 0.935568492697489, 0.341733793948539],
      [0.888797812321924, 0.376036492055393, 0.306521990587764, 0.929527518342779, 0.571114456254572]]

W1 = [[0.984958821319939, 0.652620026209849, 0.373651903167804, 0.0763713983985561],
      [0.297004413291392, 0.725685024488137, 0.133214019685439, 0.0455611437390074],
      [0.382159148226285, 0.930599850429383, 0.245580169487924, 0.25380352396242]]

W2 = [[0.77112070468893, 0.708369149485983, 0.991187379739956, 0.118948965352922],
        [0.986877727314285, 0.121819980447003, 0.586054347197509, 0.0228511834786266],
        [0.466125977018398, 0.30871369486532, 0.81602314507509, 0.673480443926495],
        [0.0233832294411814, 0.927130716720763, 0.47320919559489, 0.762412002698607],
        [0.244259662782993, 0.423909595466864, 0.301875955933872, 0.970110886125794]]

# --------------------------------------------------------------------
# Paste dataset here as:
# ([1,f1,f2,f3,f4], label)
# label in range 1..5

0.10,0.20,0.30,0.40,1
0.50,0.40,0.30,0.20,2
0.20,0.10,0.40,0.30,3
0.30,0.30,0.10,0.50,4
0.40,0.50,0.20,0.10,5
0.60,0.20,0.50,0.30,1

# --------------------------------------------------------------------
DATA = [
    ([1,0.10,0.20,0.30,0.40],1),
    ([1,0.50,0.40,0.30,0.20],2),
    ([1,0.20,0.10,0.40,0.30],3),
    ([1,0.30,0.30,0.10,0.50],4),
    ([1,0.40,0.50,0.20,0.10],5),
    ([1,0.60,0.20,0.50,0.30],1)
]

D0 = [[0]*5 for _ in range(3)]
D1 = [[0]*4 for _ in range(3)]
D2 = [[0]*4 for _ in range(5)]

lines=[]
total_cost=0.0

for idx,(a0,label) in enumerate(DATA):
    y=[0]*5
    y[label-1]=1

    z1=matvec(W0,a0)
    h1=[sigmoid(x) for x in z1]
    a1=[1]+h1

    z2=matvec(W1,a1)
    h2=[sigmoid(x) for x in z2]
    a2=[1]+h2

    z3=matvec(W2,a2)
    a3=[sigmoid(x) for x in z3]

    # cost contribution for this example:
    #   sum_k [ -y_k log(yhat_k) - (1-y_k) log(1-yhat_k) ]
    row_cost=sum(-y[k]*math.log(a3[k])-(1-y[k])*math.log(1-a3[k]) for k in range(5))
    total_cost+=row_cost

    delta3=[a3[i]-y[i] for i in range(5)]

    t=[sum(W2[r][c]*delta3[r] for r in range(5)) for c in range(4)]
    delta2=[t[i+1]*dsigmoid_from_activation(h2[i]) for i in range(3)]

    t=[sum(W1[r][c]*delta2[r] for r in range(3)) for c in range(4)]
    delta1=[t[i+1]*dsigmoid_from_activation(h1[i]) for i in range(3)]

    g0=outer(delta1,a0)
    g1=outer(delta2,a1)
    g2=outer(delta3,a2)

    for i in range(3):
        for j in range(5):
            D0[i][j]+=g0[i][j]
    for i in range(3):
        for j in range(4):
            D1[i][j]+=g1[i][j]
    for i in range(5):
        for j in range(4):
            D2[i][j]+=g2[i][j]

    lines.append("="*60)
    lines.append(f"ROW {idx}")
    lines.append("="*60)
    lines.append(f"a0={a0}")
    lines.append(f"z1={z1}")
    lines.append(f"a1={a1}")
    lines.append(f"z2={z2}")
    lines.append(f"a2={a2}")
    lines.append(f"z3={z3}")
    lines.append(f"a3={a3}")
    lines.append(f"delta3={delta3}")
    lines.append(f"delta2={delta2}")
    lines.append(f"delta1={delta1}")
    lines.append(f"Delta0={g0}")
    lines.append(f"Delta1={g1}")
    lines.append(f"Delta2={g2}")
    lines.append(f"row_cost={row_cost}")

m=max(1,len(DATA))

def update(W,D):
    out=[]
    for i,row in enumerate(W):
        nr=[]
        for j,w in enumerate(row):
            grad=D[i][j]/m
            reg=0.0
            if USE_REGULARIZATION and j>0:
                reg=(LAMBDA/m)*w
            nr.append(w-LEARNING_RATE*(grad+reg))
        out.append(nr)
    return out

newW0=update(W0,D0)
newW1=update(W1,D1)
newW2=update(W2,D2)

# --------------------------------------------------------------------
# Cost:
#   J = (1/m) * sum_i sum_k [ -y_k log(yhat_k) - (1-y_k) log(1-yhat_k) ]
#       + (lambda / 2m) * sum(theta^2)
# --------------------------------------------------------------------
sum_theta_sq=sum(w*w for W in (W0,W1,W2) for row in W for w in row)
reg_cost=(LAMBDA/(2*m))*sum_theta_sq if USE_REGULARIZATION else 0.0
cost_before_reg=total_cost/m
cost=cost_before_reg+reg_cost

lines.append("\n"+"="*60)
lines.append("COST")
lines.append("="*60)
lines.append(f"sum over examples (unnormalized) = {total_cost}")
lines.append(f"m = {m}")
lines.append(f"cost before regularization = {cost_before_reg}")
lines.append(f"sum(theta^2) = {sum_theta_sq}")
lines.append(f"lambda = {LAMBDA}, use_regularization = {USE_REGULARIZATION}")
lines.append(f"regularization term = {reg_cost}")
lines.append(f"J(theta) = {cost}")

lines.append("\nAccumulated Delta0="+str(D0))
lines.append("Accumulated Delta1="+str(D1))
lines.append("Accumulated Delta2="+str(D2))
lines.append("\nUpdated W0="+str(newW0))
lines.append("Updated W1="+str(newW1))
lines.append("Updated W2="+str(newW2))

with open("nn_full_training_trace.txt","w") as f:
    f.write("\n".join(lines))

print("Generated nn_full_training_trace.txt")
