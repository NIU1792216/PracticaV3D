# Pràctica 1
## Pregunta 1
Defineix les comandes `glViewport()`, `glOrtho()` i `gluLookAt()` per a visualitzar
les escenes de la figura 16 amb $w<h$ per a encabir una esfera de radi 5.

```c++
glScissor(0, 0, w/2, h);
glViewport(0, 0, w/2, h);
double aspecte = (float)w/(float)h;
glOrtho(-Raux*aspecte, Raux*aspecte, -Raux, Raux, -p_far, p_far);
gluLookAt(Raux, Raux, Raux, 0., 0., 0., 2, -1., -1.)
```

```c++
glScissor(0, w/2, w/2, h);
glViewport(0, w/2, w/2, h);
double aspecte = (float)w/(float)h;
glOrtho(-Raux*aspecte, Raux*aspecte, -Raux, Raux, -p_far, p_far);
gluLookAt(0., 0., Raux, 0., 0., 0., -1., 0., 0.)
```
On Raux depén del tamany de l'objecte a visualitzar. en el nostre cas `Raux= 10`.
## Pregunta 2
De la figura 16, la vista B, justifica com has calculat el vector tangent v de la càmera?

Sabent que el vector ha de ser perpendicular al que uneix la càmara amb el centre (-Raux, -Raux, -Raux) i que ha de estar en el pla que conté l'eix X i la càmara, podem fixar les coordenades y, z a -1, per tal que estigui al pla i "apunti" cap al centre i després trobem x per tal que el producte escalar doni zero, que siguin perpendiculars. Així trobem el (2, -1, -1).
