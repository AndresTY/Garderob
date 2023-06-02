from django.urls import path
from . import views

urlpatterns = [
    path('', views.home, name='armario-home'),
    path('about/', views.about, name='armario-about'),
    path("img", views.img_call, name="img-call"),
    path("view", views.view_img, name="armario-view"),
    path("working", views.working, name="armario-work")
]